# Guitar Diagnostic Analyzer

Professional C++20 real-time guitar diagnostics application using advanced DSP algorithms.

## Project Status

**Phase 3 Complete** ✅ - All three diagnostic analyzers fully implemented and tested

- ✅ **Phase 1**: Foundation (core interfaces, build system)
- ✅ **Phase 2**: Audio infrastructure (real-time pipeline, lock-free threading)
- ✅ **Phase 3**: Diagnostic analyzers (Fret Buzz, Intonation, String Health)
- ⏳ **Phase 4**: UI layer (pending)

## Features

### Implemented Analyzers

#### 1. **Fret Buzz Detector**

Detects fret buzz using transient and spectral analysis:

- Onset detection (RMS energy + spectral flux)
- Transient analysis (attack time, zero-crossing rate)
- High-frequency noise detection (4-8 kHz band)
- Inharmonicity measurement
- **Score**: `0.3×transient + 0.4×highFreq + 0.3×inharmonicity`

#### 2. **Intonation Analyzer**

Measures string intonation accuracy with state machine:

- YIN pitch detection (±0.1 cent accuracy)
- State machine: Idle → OpenString → 12thFret → FrettedString → Complete
- Pitch stabilization (median filter + 500ms accumulation)
- Cent deviation calculation: `1200 × log₂(measured/expected)`
- ±5 cent tolerance for in-tune detection

#### 3. **String Health Analyzer**

Assesses string condition through harmonic analysis:

- Harmonic decay tracking (10 harmonics, exponential fit)
- Spectral features (centroid, rolloff, brightness)
- Inharmonicity measurement
- **Health Score**: `0.3×decay + 0.3×spectral + 0.4×inharmonicity`

### Architecture

**Three-Thread Model** (real-time safe):

```text
AudioProcessingLayer (RT Thread)
    ↓ lock-free writes
LockFreeRingBuffer<float> (SPSC)
    ↓ lock-free reads
AnalysisEngine (Worker Thread)
    ↓ orchestrates
[FretBuzzDetector, IntonationAnalyzer, StringHealthAnalyzer]
    ↓ atomic shared_ptr
UI Thread (read-only)
```

**Key Design Principles**:

- **Real-time safety**: No allocations in audio callback
- **Lock-free communication**: SPSC ring buffer
- **Thread-safe results**: Mutex-protected shared_ptr swap
- **Pre-allocated buffers**: All memory allocated in constructors

## Technology Stack

- **Language**: C++20 (gcc 11+, clang 13+, MSVC 19.29+)
- **Framework**: [kappa-core](https://github.com/Konstantysz/kappa-core) (OpenGL, GLFW, ImGui)
- **Audio I/O**: [lib-guitar-io](https://github.com/Konstantysz/lib-guitar-io) (RtAudio wrapper)
- **DSP**: [lib-guitar-dsp](https://github.com/Konstantysz/lib-guitar-dsp) (YIN, PFFFT)
- **Build**: CMake 3.20+
- **Testing**: Google Test
- **Dependencies**: vcpkg + git submodules

## Building

### Prerequisites

```bash
# Install vcpkg dependencies (automatic via CMake)
# - glad, glfw3, glm, nlohmann-json, spdlog, gtest

# Clone with submodules
git clone --recursive https://github.com/Konstantysz/guitar-diagnostics.git
cd guitar-diagnostics
```

### Build Commands

⚠️ **Note**: As per CLAUDE.md policy, AI assistants do NOT run build commands. Developers control when to build/test.

#### Using CMake Presets (Recommended)

The project includes CMake presets for easy configuration:

```bash
# Configure for Release (Windows)
cmake --preset windows-release

# Build
cmake --build --preset windows-release

# Run tests
ctest --preset windows-release

# Or for Debug
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug
```

**Available presets**:

- `windows-release` / `windows-debug` - Windows builds
- `linux-release` / `linux-debug` - Linux builds
- `macos-release` / `macos-debug` - macOS builds
- `windows-debug-asan` - Windows Debug with AddressSanitizer

#### Manual Build (Alternative)

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

# Build
cmake --build build --config Release

# Run tests
ctest --test-dir build -C Release --output-on-failure
```

### CMake Options

```cmake
-DGD_ENABLE_WARNINGS=ON    # Strict compiler warnings (default: ON)
-DGD_ENABLE_ASAN=ON        # AddressSanitizer (default: OFF)
-DCMAKE_BUILD_TYPE=Release # Release, Debug, RelWithDebInfo
```

## VS Code Integration

The project includes complete VS Code integration for seamless development.

### CMake Tools Integration

Configured in [.vscode/settings.json](.vscode/settings.json):

- **Preset-based workflow**: `"cmake.useCMakePresets": "always"`
- **Manual control**: `"cmake.configureOnOpen": false`
- **Auto-build**: `"cmake.buildBeforeRun": true`
- **C++20 IntelliSense**: Full language support with `"C_Cpp.default.cppStandard": "c++20"`

### Quick Build Tasks

Configured in [.vscode/tasks.json](.vscode/tasks.json), accessible via **Ctrl+Shift+P** → "Tasks: Run Task":

**Configuration Tasks**:

- `CMake: Configure (Release)` - Configure with Release preset
- `CMake: Configure (Debug)` - Configure with Debug preset

**Build Tasks**:

- `CMake: Build (Release)` - **Default build** (Ctrl+Shift+B)
- `CMake: Build (Debug)` - Debug build
- `CMake: Clean Build (Release)` - Clean and rebuild

**Test Tasks**:

- `CTest: Run All Tests (Release)` - Run all 56 tests
- `CTest: Run IntonationAnalyzer Tests` - Intonation tests only
- `CTest: Run FretBuzzDetector Tests` - Fret buzz tests only
- `CTest: Run StringHealthAnalyzer Tests` - String health tests only
- `CTest: Run Integration Tests` - Integration pipeline tests

**Combined Tasks**:

- `Build and Test (Release)` - Build + run all tests sequentially

### CMake Presets in VS Code

Use the **CMake Tools extension** for GUI-based preset selection:

1. **Select Preset**: Click CMake preset in status bar → choose `windows-release`
2. **Configure**: Click "Configure" in status bar (or use task)
3. **Build**: Click "Build" in status bar (or press Ctrl+Shift+B)
4. **Test**: Click "Test" in status bar (or use task)

### Keyboard Shortcuts

- **Ctrl+Shift+B** - Default build (Release)
- **Ctrl+Shift+P** → "Tasks: Run Task" - Access all tasks
- **Ctrl+Shift+P** → "CMake: Select Configure Preset" - Change preset

## Testing

**Test Coverage** (Phase 3):

- **Unit Tests**: 31 analyzer tests
- **Integration Tests**: 7 pipeline tests
- **Infrastructure Tests**: 18 foundation tests
- **Total**: 56 tests

**Run specific test suites**:

```bash
ctest -R IntonationAnalyzer  # Intonation tests only
ctest -R FretBuzzDetector    # Fret buzz tests only
ctest -R StringHealth        # String health tests only
ctest -R Integration         # Integration tests
```

## Project Structure

```text
guitar-diagnostics/
├── src/
│   ├── Analysis/
│   │   ├── Analyzer.h                    # Base analyzer interface
│   │   ├── AnalysisEngine.{h,cpp}        # Worker thread orchestration
│   │   ├── Fretbuzz/
│   │   │   └── FretBuzzDetector.{h,cpp}  # ✅ Phase 3
│   │   ├── Intonation/
│   │   │   └── IntonationAnalyzer.{h,cpp} # ✅ Phase 3
│   │   └── StringHealth/
│   │       └── StringHealthAnalyzer.{h,cpp} # ✅ Phase 3
│   ├── App/
│   │   └── AudioProcessingLayer.{h,cpp}  # ✅ Phase 2
│   ├── Audio/
│   │   └── AudioDeviceManager.{h,cpp}    # ✅ Phase 2
│   └── Util/
│       └── LockFreeRingBuffer.h          # ✅ Phase 2
│
├── tests/
│   ├── Analysis/
│   │   ├── TestFretBuzzDetector.cpp      # ✅ 11 tests
│   │   ├── TestIntonationAnalyzer.cpp    # ✅ 11 tests
│   │   └── TestStringHealthAnalyzer.cpp  # ✅ 9 tests
│   └── Integration/
│       └── TestAnalysisPipeline.cpp      # ✅ 7 tests
│
├── external/                              # Git submodules
│   ├── kappa-core/
│   ├── lib-guitar-io/
│   └── lib-guitar-dsp/
│
├── CLAUDE.md                              # Development guide
└── README.md                              # This file
```

## Performance Targets

- **CPU Usage**: <7% total (i5/i7)
  - IntonationAnalyzer: <1%
  - FretBuzzDetector: <3%
  - StringHealthAnalyzer: <3%
- **Memory**: <500KB for all analyzers
- **Latency**: <20ms (audio input → UI display)
- **Buffer Size**: 2048 samples @ 48kHz (43ms)

## DSP Algorithms

### Fret Buzz Detection

**Algorithm**: Transient + Spectral Anomaly + Inharmonicity

1. **Onset Detection**: RMS energy ratio + spectral flux
2. **Transient Analysis**: Attack time (<0.1s) + zero-crossing rate
3. **Spectral Anomalies**: 4-8 kHz band energy ratio
4. **Inharmonicity**: Harmonic deviation from ideal positions
5. **Scoring**: `0.3×transient + 0.4×highFreq + 0.3×inharmonic`

### Intonation Analysis

**Algorithm**: YIN Pitch Tracking + State Machine

1. **State Machine**: Idle → OpenString → WaitFor12thFret → FrettedString → Complete
2. **Pitch Tracking**: YIN algorithm (2048 samples, 0.15 threshold)
3. **Stability**: Median filter + 500ms accumulation
4. **Deviation**: `cents = 1200 × log₂(fretted / (2 × open))`
5. **Tolerance**: ±5 cents

### String Health Analysis

**Algorithm**: Harmonic Decay + Spectral Features

1. **Harmonic Tracking**: f₀, 2f₀, ..., 10f₀ over 50 frames
2. **Decay Fitting**: Exponential fit → dB/s rate
3. **Spectral Features**: Centroid (brightness), rolloff
4. **Inharmonicity**: Peak deviation from ideal harmonics
5. **Health Score**: `0.3×decay + 0.3×spectral + 0.4×inharmonic`

## Coding Standards

See [CLAUDE.md](CLAUDE.md) for complete development guide.

**Critical Rules**:

- ✅ PascalCase: classes, functions, namespaces
- ✅ camelCase: all variables (no `m_` prefix)
- ✅ Constructor initializer lists (no in-class initialization)
- ✅ NO implementations in headers
- ✅ NO allocations in real-time audio callback
- ✅ `#pragma once` header guards
- ✅ Functions ≤100 lines

## Contributing

1. Follow TDD: Write tests first, then implementation
2. Adhere to coding standards in CLAUDE.md
3. Ensure all tests pass before committing
4. Single-line commit messages (imperative mood)
5. No force pushes to main/master

## Roadmap

### Phase 4 (Pending)

- [ ] Main application class
- [ ] DiagnosticVisualizationLayer (ImGui integration)
- [ ] TabController (UI panel management)
- [ ] Four UI panels:
  - [ ] FretBuzzPanel
  - [ ] IntonationPanel
  - [ ] StringHealthPanel
  - [ ] AudioMonitorPanel
- [ ] Complete main() entry point

### Future Enhancements

- [ ] Audio file input (WAV/FLAC)
- [ ] Export analysis results (JSON/CSV)
- [ ] Preset management
- [ ] Multi-string analysis
- [ ] Real-time waveform visualization

## License

MIT License - See [LICENSE](LICENSE) file

## References

- [YIN Algorithm Paper](http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf) - Pitch detection
- [PFFFT Library](https://bitbucket.org/jpommier/pffft) - Fast FFT
- [kappa-core Documentation](https://github.com/Konstantysz/kappa-core)
- [lib-guitar-dsp Documentation](https://github.com/Konstantysz/lib-guitar-dsp)
- [precision-guitar-tuner](https://github.com/Konstantysz/precision-guitar-tuner) - Reference implementation

## Acknowledgments

Built with:

- **kappa-core** - OpenGL/ImGui application framework
- **lib-guitar-io** - Cross-platform audio I/O
- **lib-guitar-dsp** - Guitar-specific DSP algorithms
- **PFFFT** - Fast Fourier Transform library
- **RtAudio** - Real-time audio I/O

---

**Current Status**: Phase 3 complete - All three analyzers implemented and tested ✅
