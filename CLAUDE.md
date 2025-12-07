# Guitar Diagnostic Analyzer - Development Guide

## Project Overview

**Guitar Diagnostic Analyzer** is a professional C++20 application for real-time guitar diagnostics using advanced DSP algorithms. It provides three core analysis modules: Fret Buzz Detection, String Intonation Analysis, and String Health Assessment.

### Key Features

- **Real-time Audio Processing**: Low-latency audio input via ASIO/CoreAudio/ALSA
- **Three Diagnostic Modules**:
  - Fret Buzz Detector (transient + spectral analysis)
  - Intonation Analyzer (YIN pitch tracking + state machine)
  - String Health Analyzer (harmonic decay + spectral brightness)
- **Tab-Based UI**: Four panels (Fret Buzz, Intonation, String Health, Audio Monitor)
- **Thread-Safe Architecture**: Separate real-time audio, analysis worker, and UI threads

## Architecture

### High-Level Design

```
Application (kappa-core)
    ├─→ AudioProcessingLayer (Real-time Thread)
    │       ↓
    │   lib-guitar-io → Lock-Free Ring Buffer
    │
    └─→ DiagnosticVisualizationLayer (Main Thread)
            ↓
        TabController → 4 Panels
            ↓
        AnalysisEngine (Worker Thread)
            ├─→ FretBuzzDetector
            ├─→ IntonationAnalyzer
            └─→ StringHealthAnalyzer
                    ↓
                lib-guitar-dsp (YIN, FFT)
```

### Thread Safety Model

**Real-time Audio Thread**:
- Highest priority
- NO allocations, NO mutexes, NO I/O
- Lock-free writes to ring buffer only

**Analysis Worker Thread**:
- Normal priority
- Reads from ring buffer
- Can allocate, use mutexes, log

**Main/UI Thread**:
- Normal priority
- Read-only access to results via atomic shared_ptr
- 60 FPS target

### Core Components

1. **AudioProcessingLayer**: Real-time audio callback, writes to ring buffer
2. **AnalysisEngine**: Worker thread, orchestrates analyzers
3. **Analyzers**: FretBuzzDetector, IntonationAnalyzer, StringHealthAnalyzer
4. **DiagnosticVisualizationLayer**: Main UI layer
5. **TabController**: Manages 4 UI panels
6. **LockFreeRingBuffer**: SPSC queue for thread communication

## Technology Stack

- **Language**: C++20 (gcc 11+, clang 13+, MSVC 19.29+)
- **Framework**: kappa-core (OpenGL 3.3+, GLFW, ImGui)
- **Audio I/O**: lib-guitar-io (RtAudio wrapper)
- **DSP**: lib-guitar-dsp (YIN pitch detection, PFFFT)
- **Build System**: CMake 3.20+
- **Testing**: Google Test
- **Dependencies**: vcpkg or git submodules

## Coding Standards

### File Organization

- **Headers**: All headers (.h files) are placed in `src/` directory, NOT in `include/`
- **Header Guards**: Use `#pragma once` (NOT `#ifndef` guards)
- **Directory Structure**: Headers in category folders like `src/Analysis/`, `src/UI/`, `src/Util/`

### Naming Conventions

```cpp
// PascalCase for classes, structs, functions, AND NAMESPACES
namespace GuitarDiagnostics::Analysis  // ✅ CORRECT - PascalCase namespaces
{
    class AudioProcessingLayer;
    struct AnalysisConfig;
    float GetLatestPitch();
}

// camelCase for ALL variables (including class members)
int sampleRate;
float pitchEstimate;
AudioDevice *audioDevice;  // NO m_ prefix, NO trailing underscore

// Bastardized Hungarian ONLY for global constants
constexpr size_t g_kMaxBufferSize = 4096;
constexpr float g_kDefaultSampleRate = 48000.0f;

// Abstract base classes: Use descriptive names WITHOUT "I" prefix
class Analyzer;     // ✅ CORRECT (not IAnalyzer)
class Panel;        // ✅ CORRECT (not IPanel)
```

### Namespace Rules

```cpp
// ✅ CORRECT - PascalCase, never use "using namespace"
namespace GuitarDiagnostics::Analysis
{
    class FretBuzzDetector : public Analyzer
    {
        // ...
    };
}

// Usage in implementation files
GuitarDiagnostics::Analysis::FretBuzzDetector detector;

// ❌ WRONG - Never use "using namespace" (exception: test files)
using namespace GuitarDiagnostics;  // FORBIDDEN in production code
```

### Critical Rules

✅ **ALWAYS use constructor initializer lists**
```cpp
// ✅ CORRECT
class FretBuzzDetector
{
public:
    FretBuzzDetector(float sampleRate, size_t fftSize);

private:
    float sampleRate;      // NO initialization here
    size_t fftSize;        // NO initialization here
    std::vector<float> buffer;
};

// In .cpp file:
FretBuzzDetector::FretBuzzDetector(float sampleRate, size_t fftSize)
    : sampleRate(sampleRate)
    , fftSize(fftSize)
    , buffer(fftSize, 0.0f)
{
    // Constructor body (if needed)
}

// ❌ WRONG - No in-class initialization
class FretBuzzDetector
{
private:
    float sampleRate = 48000.0f;  // NEVER DO THIS
    size_t fftSize{2048};          // NEVER DO THIS
};
```

❌ **NEVER implement methods in headers (even one-liners)**
```cpp
// ❌ WRONG - Implementation in header
class AudioDevice
{
public:
    bool IsRunning() const { return isRunning; }  // NEVER DO THIS

private:
    bool isRunning;
};

// ✅ CORRECT - Declaration in header, implementation in .cpp
// AudioDevice.h
class AudioDevice
{
public:
    bool IsRunning() const;

private:
    bool isRunning;
};

// AudioDevice.cpp
bool AudioDevice::IsRunning() const
{
    return isRunning;
}
```

❌ **NEVER initialize class variables in headers**
- All initialization must happen in constructor initializer list

**Functions must be ≤100 lines**
- If longer, refactor into smaller functions
- Use helper functions for complex logic

### Code Formatting

```cpp
// Line limit: 120 characters
// Indentation: 4 spaces (NO tabs)
// Brace style: Allman (opening braces on new lines)

void ProcessAudioBuffer(std::span<const float> input, std::span<float> output)
{
    for (size_t i = 0; i < input.size(); ++i)
    {
        float sample = input[i];

        if (sample > threshold)
        {
            output[i] = ApplyGain(sample);
        }
        else
        {
            output[i] = 0.0f;
        }
    }
}

// Pointer alignment: Right-aligned
AudioDevice *device;        // ✅ CORRECT
float *buffer;              // ✅ CORRECT
const char *message;        // ✅ CORRECT

AudioDevice* device;        // ❌ WRONG
float* buffer;              // ❌ WRONG
```

### Real-Time Audio Constraints

❌ **UNSAFE in audio callback** (absolutely forbidden):
```cpp
void OnAudioInput(std::span<const float> input)
{
    // ❌ NO ALLOCATIONS
    new float[1024];                    // FORBIDDEN
    delete[] buffer;                    // FORBIDDEN
    malloc(1024);                       // FORBIDDEN
    std::vector<float> temp;            // FORBIDDEN (might allocate)
    temp.push_back(1.0f);              // FORBIDDEN (allocates)
    std::string msg = "test";          // FORBIDDEN (allocates)
    msg += "more";                     // FORBIDDEN (allocates)

    // ❌ NO MUTEXES
    std::mutex mutex;                  // FORBIDDEN
    std::lock_guard<std::mutex> lock;  // FORBIDDEN

    // ❌ NO I/O
    std::cout << "audio";              // FORBIDDEN
    spdlog::info("audio");             // FORBIDDEN (even fast loggers)
    std::ofstream file("data.txt");    // FORBIDDEN
}
```

✅ **SAFE in audio callback**:
```cpp
void OnAudioInput(std::span<const float> input)
{
    // ✅ Pre-allocated buffers
    for (size_t i = 0; i < input.size(); ++i)
    {
        preallocatedBuffer[i] = input[i];
    }

    // ✅ Lock-free operations
    ringBuffer.Write(input);  // Lock-free SPSC queue

    // ✅ Atomic operations
    frameCount.fetch_add(1, std::memory_order_relaxed);

    // ✅ Simple arithmetic
    float rms = CalculateRMS(input);  // Pure computation, no allocations
}
```

## Git Commit Guidelines

**Single-line commit messages only** (imperative mood):

```bash
# ✅ CORRECT
git commit -m "Add FretBuzzDetector with onset detection"
git commit -m "Fix buffer overflow in ring buffer write"
git commit -m "Implement IntonationAnalyzer state machine"
git commit -m "Refactor AnalysisEngine thread management"

# ❌ WRONG
git commit -m "Added FretBuzzDetector"           # Not imperative
git commit -m "I fixed the bug"                  # Not imperative
git commit -m "Add feature\n\nLong description"  # Multi-line
```

## Development Practices

### Test-Driven Development (TDD)

**ALWAYS write tests first:**

1. Write failing test
2. Run test, verify it fails
3. Write minimal code to pass
4. Run test, verify it passes
5. Refactor while keeping tests green

Example workflow:
```cpp
// Step 1: Write failing test
TEST(LockFreeRingBufferTest, WriteAndReadSingleElement)
{
    LockFreeRingBuffer<float> buffer(1024);

    std::array<float, 1> input = {42.0f};
    ASSERT_TRUE(buffer.Write(input));

    std::array<float, 1> output;
    ASSERT_EQ(buffer.Read(output), 1);
    EXPECT_FLOAT_EQ(output[0], 42.0f);
}

// Step 2: Run test (should FAIL - no implementation yet)

// Step 3: Implement minimal code
template<typename T>
bool LockFreeRingBuffer<T>::Write(std::span<const T> data)
{
    // ... implementation ...
}

// Step 4: Run test (should PASS)

// Step 5: Refactor if needed
```

### Design Principles

**SOLID**:
- **S**ingle Responsibility: Each class has one reason to change
- **O**pen/Closed: Open for extension, closed for modification
- **L**iskov Substitution: Derived classes are substitutable
- **I**nterface Segregation: Small, focused interfaces
- **D**ependency Inversion: Depend on abstractions, not concretions

**DRY** (Don't Repeat Yourself):
- Extract common code into functions
- Use templates for type-generic code
- Avoid copy-paste programming

**KISS** (Keep It Simple, Stupid):
- Prefer simple solutions over clever ones
- Avoid premature optimization
- Write readable, maintainable code

**YAGNI** (You Aren't Gonna Need It):
- Don't add features for hypothetical future needs
- Build what's needed NOW
- Refactor when requirements change

## Directory Structure

```
guitar-diagnostics/
├── CMakeLists.txt              # Root build configuration
├── CLAUDE.md                   # This file
├── README.md                   # User-facing documentation
├── LICENSE                     # MIT License
├── .gitignore                  # Git ignore rules
│
├── external/                   # Git submodules
│   ├── kappa-core/
│   ├── lib-guitar-io/
│   └── lib-guitar-dsp/
│
├── src/                        # Implementation files
│   ├── GuitarDiagnostics.cpp
│   ├── App/
│   │   ├── Application.cpp
│   │   ├── AudioProcessingLayer.cpp
│   │   └── DiagnosticVisualizationLayer.cpp
│   ├── Analysis/
│   │   ├── Analyzer.h
│   │   ├── AnalysisEngine.h
│   │   ├── AnalysisEngine.cpp
│   │   ├── AnalysisResult.h
│   │   ├── Fretbuzz/
│   │   │   ├── FretBuzzDetector.h
│   │   │   └── FretBuzzDetector.cpp
│   │   ├── Intonation/
│   │   │   ├── IntonationAnalyzer.h
│   │   │   └── IntonationAnalyzer.cpp
│   │   └── StringHealth/
│   │       ├── StringHealthAnalyzer.h
│   │       └── StringHealthAnalyzer.cpp
│   ├── Audio/
│   │   └── AudioDeviceManager.cpp
│   ├── UI/
│   │   ├── TabController.cpp
│   │   └── panels/
│   │       ├── FretBuzzPanel.cpp
│   │       ├── IntonationPanel.cpp
│   │       ├── StringHealthPanel.cpp
│   │       └── AudioMonitorPanel.cpp
│   └── Util/
│       └── SignalGenerator.cpp
│
├── tests/                      # Google Test suites
│   ├── CMakeLists.txt
│   ├── Analysis/
│   │   ├── TestFretbuzzRetector.cpp
│   │   ├── TestIntonationAnalyzer.cpp
│   │   └── TestStringhealthAnalyzer.cpp
│   ├── Audio/
│   │   └── TestLockFreeRingBuffer.cpp
│   ├── Integration/
│   │   └── TestAnalysisPipeline.cpp
│   └── Util/
│       └── TestSignalGenerator.cpp
│
└── assets/                     # Resources (future)
    ├── shaders/
    └── fonts/
```

## Build Instructions

### ⚠️ CRITICAL: Build Policy for AI Assistants

**Claude (or any AI assistant) must NEVER run these commands unless explicitly instructed by the developer:**

- ❌ `cmake ..` (CMake configuration)
- ❌ `cmake --build .` (compilation)
- ❌ `ctest` (test execution)
- ❌ Any build, test, or runtime execution

**Claude's role**: Write code, tests, and documentation. **Developer controls** when to build/test.

### For Developers Only

```bash
# Clone repository with submodules
git clone --recursive https://github.com/Konstantysz/guitar-diagnostics.git
cd guitar-diagnostics

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Run tests
ctest -C Release

# Run application
./bin/Release/GuitarDiagnostics
```

## DSP Algorithms

### Fret Buzz Detection

**Algorithm**: Transient + Spectral Anomaly + Inharmonicity

1. **Onset Detection**: RMS energy + spectral flux
2. **Transient Analysis**: Attack time, zero-crossing rate
3. **Spectral Anomalies**: High-frequency energy (4-8 kHz)
4. **Inharmonicity**: Deviation from ideal harmonics
5. **Scoring**: `0.3*transient + 0.4*highFreqNoise + 0.3*inharmonicity`

### Intonation Analysis

**Algorithm**: YIN Pitch Tracking + State Machine

1. **State Machine**: Idle → OpenString → WaitFor12thFret → FrettedString → Complete
2. **Pitch Tracking**: YIN algorithm with 2048 sample window
3. **Averaging**: Median filter + 500ms stable accumulation
4. **Deviation**: `cents = 1200 * log2(measured / expected)`
5. **Tolerance**: ±5 cents

### String Health Analysis

**Algorithm**: Harmonic Decay + Spectral Brightness + Inharmonicity

1. **Harmonic Extraction**: Track f₀, 2f₀, ..., 10f₀
2. **Decay Analysis**: Exponential fit, measure dB/s
3. **Spectral Features**: Centroid, rolloff, HF energy
4. **Inharmonicity**: `β = Σ|f'_n - n*f₀| / (n*f₀)`
5. **Health Score**: `0.3*decay + 0.3*spectral + 0.4*inharmonicity`

## Performance Targets

- **CPU Usage**: <10% on modern i5/i7
- **Memory**: <100MB RAM
- **Latency**: <20ms total (input → display)
- **Frame Rate**: 60 FPS UI rendering
- **Audio Buffer**: 512 samples @ 48kHz (10.7ms)

## Common Pitfalls

### Pitfall #1: Member Variable Initialization

❌ **WRONG**:
```cpp
class Analyzer
{
private:
    float sampleRate = 48000.0f;  // NEVER in header
};
```

✅ **CORRECT**:
```cpp
// Analyzer.h
class Analyzer
{
public:
    Analyzer(float sampleRate);
private:
    float sampleRate;  // No initialization
};

// Analyzer.cpp
Analyzer::Analyzer(float sampleRate)
    : sampleRate(sampleRate)  // Initialize here
{
}
```

### Pitfall #2: Methods in Headers

❌ **WRONG**:
```cpp
class AudioDevice
{
public:
    void Start() { isRunning = true; }  // NEVER in header
};
```

✅ **CORRECT**:
```cpp
// AudioDevice.h
class AudioDevice
{
public:
    void Start();
};

// AudioDevice.cpp
void AudioDevice::Start()
{
    isRunning = true;
}
```

### Pitfall #3: Allocations in Audio Callback

❌ **WRONG**:
```cpp
void OnAudio(std::span<const float> input)
{
    std::vector<float> temp(input.size());  // ALLOCATION!
    // ...
}
```

✅ **CORRECT**:
```cpp
class AudioProcessor
{
public:
    AudioProcessor(size_t maxBufferSize)
        : tempBuffer(maxBufferSize)  // Pre-allocate in constructor
    {
    }

    void OnAudio(std::span<const float> input)
    {
        // Use pre-allocated buffer
        std::copy(input.begin(), input.end(), tempBuffer.begin());
    }

private:
    std::vector<float> tempBuffer;
};
```

## Troubleshooting

### Build Issues

**CMake can't find dependencies**:
```bash
git submodule update --init --recursive
```

**C++20 not available**:
- Update compiler: gcc 11+, clang 13+, MSVC 19.29+

### Runtime Issues

**Audio clicks/pops**:
- Check for allocations in audio callback
- Reduce buffer size (512 → 256 samples)
- Increase thread priority

**High CPU usage**:
- Profile with tools (perf, Instruments, VTune)
- Optimize FFT size
- Reduce analysis rate

## References

- [kappa-core](https://github.com/Konstantysz/kappa-core) - Application framework
- [lib-guitar-io](https://github.com/Konstantysz/lib-guitar-io) - Audio I/O wrapper
- [lib-guitar-dsp](https://github.com/Konstantysz/lib-guitar-dsp) - DSP algorithms
- [precision-guitar-tuner](https://github.com/Konstantysz/precision-guitar-tuner) - Reference implementation
- [YIN algorithm](http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf) - Pitch detection paper
- [RtAudio](https://www.music.mcgill.ca/~gary/rtaudio/) - Cross-platform audio I/O

## Contact

For questions or contributions, please open an issue on GitHub.
