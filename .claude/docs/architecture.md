# Architecture

## High-Level Design

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

## Thread Safety Model

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

## Core Components

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

## Directory Structure

```
guitar-diagnostics/
├── CMakeLists.txt              # Root build configuration
├── CLAUDE.md                   # Main project guide
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

## Performance Targets

- **CPU Usage**: <10% on modern i5/i7
- **Memory**: <100MB RAM
- **Latency**: <20ms total (input → display)
- **Frame Rate**: 60 FPS UI rendering
- **Audio Buffer**: 512 samples @ 48kHz (10.7ms)
