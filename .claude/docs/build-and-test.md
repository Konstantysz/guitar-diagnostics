# Build & Test Workflow

## ⚠️ CRITICAL: Build Policy for AI Assistants

**Claude (or any AI assistant) must NEVER run these commands unless explicitly instructed by the developer:**

- ❌ `cmake ..` (CMake configuration)
- ❌ `cmake --build .` (compilation)
- ❌ `ctest` (test execution)
- ❌ Any build, test, or runtime execution

**Claude's role**: Write code, tests, and documentation. **Developer controls** when to build/test.

## Build Instructions (For Developers Only)

### Using CMake Presets (Recommended)

```bash
# Clone repository with submodules
git clone --recursive https://github.com/Konstantysz/guitar-diagnostics.git
cd guitar-diagnostics

# Configure with preset
cmake --preset windows-release  # or linux-release, macos-release

# Build
cmake --build --preset windows-release

# Run tests
ctest --preset windows-release

# Run application
./build/windows-release/bin/Release/GuitarDiagnostics
```

**Available presets**: windows-release, windows-debug, linux-release, linux-debug, macos-release, macos-debug, windows-debug-asan

### Manual Build (Alternative)

```bash
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

## Test-Driven Development (TDD)

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

## VS Code Integration

The project includes complete VS Code integration for modern C++ development.

### Setup Requirements

Install these VS Code extensions:

- **C/C++** (ms-vscode.cpptools) - IntelliSense, debugging
- **CMake Tools** (ms-vscode.cmake-tools) - CMake integration
- **CMake** (twxs.cmake) - CMake syntax highlighting

### Configuration Files

#### `.vscode/settings.json`

Configures CMake Tools and C++ IntelliSense:

```json
{
  "cmake.useCMakePresets": "always",       // Use CMakePresets.json
  "cmake.configureOnOpen": false,          // Manual configuration control
  "cmake.buildBeforeRun": true,            // Auto-build before debugging
  "C_Cpp.default.cppStandard": "c++20",    // C++20 IntelliSense
  "C_Cpp.default.intelliSenseMode": "windows-msvc-x64"
}
```

#### `.vscode/tasks.json`

Provides 12 quick-access tasks:

**Configuration**:
- `CMake: Configure (Release)` / `(Debug)`

**Building**:
- `CMake: Build (Release)` - **Default** (Ctrl+Shift+B)
- `CMake: Build (Debug)`
- `CMake: Clean Build (Release)`

**Testing**:
- `CTest: Run All Tests (Release)` / `(Debug)`
- `CTest: Run IntonationAnalyzer Tests`
- `CTest: Run FretBuzzDetector Tests`
- `CTest: Run StringHealthAnalyzer Tests`
- `CTest: Run Integration Tests`

**Combined**:
- `Build and Test (Release)` - Sequential build + test

#### `CMakePresets.json`

Defines platform-specific build configurations:

```json
{
  "configurePresets": [
    {"name": "windows-release", "binaryDir": "build/windows-release"},
    {"name": "windows-debug", "binaryDir": "build/windows-debug"},
    {"name": "linux-release", "binaryDir": "build/linux-release"},
    {"name": "macos-release", "binaryDir": "build/macos-release"},
    {"name": "windows-debug-asan", "cacheVariables": {"GD_ENABLE_ASAN": "ON"}}
  ]
}
```

### Workflow

#### Method 1: CMake Tools GUI (Recommended)

1. **Select Preset**: Click CMake preset in status bar → `windows-release`
2. **Configure**: Click "Configure" button in status bar
3. **Build**: Click "Build" button (or press Ctrl+Shift+B)
4. **Test**: Click "Test" button in status bar
5. **Debug**: Set breakpoints, press F5

#### Method 2: Tasks (Keyboard-Driven)

1. **Configure**: Ctrl+Shift+P → "Tasks: Run Task" → "CMake: Configure (Release)"
2. **Build**: Ctrl+Shift+B (default build task)
3. **Test**: Ctrl+Shift+P → "Tasks: Run Task" → "CTest: Run All Tests (Release)"

#### Method 3: Command Palette

- **Ctrl+Shift+P** → "CMake: Configure"
- **Ctrl+Shift+P** → "CMake: Build"
- **Ctrl+Shift+P** → "CMake: Run Tests"

### Keyboard Shortcuts

- **Ctrl+Shift+B** - Default build (Release)
- **Ctrl+Shift+P** - Command palette (access all CMake/task commands)
- **F5** - Start debugging (builds first if needed)
- **Ctrl+F5** - Run without debugging

### IntelliSense Configuration

IntelliSense is automatically configured by CMake Tools:

- **Include paths**: Auto-detected from CMake
- **Defines**: Auto-detected from CMake
- **Compiler**: MSVC on Windows, GCC/Clang on Linux/macOS
- **Standard**: C++20 (configured in settings.json)

**If IntelliSense breaks**:
1. Ctrl+Shift+P → "C/C++: Reset IntelliSense Database"
2. Ctrl+Shift+P → "CMake: Delete Cache and Reconfigure"

### Debugging

CMake Tools auto-generates `.vscode/launch.json` when you first debug:

1. Set breakpoints in code (click left margin)
2. Press **F5** (or Ctrl+Shift+P → "CMake: Debug")
3. Debugger attaches with full symbol information

**Manual debugging** (if auto-gen fails):

```json
// .vscode/launch.json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug GuitarDiagnostics",
      "type": "cppvsdbg",  // or "cppdbg" for GDB/LLDB
      "request": "launch",
      "program": "${workspaceFolder}/build/windows-debug/bin/Debug/GuitarDiagnostics.exe",
      "cwd": "${workspaceFolder}",
      "preLaunchTask": "CMake: Build (Debug)"
    }
  ]
}
```

### Troubleshooting VS Code Integration

**"CMake Tools could not find CMake"**:
- Install CMake 3.20+ and add to PATH
- Restart VS Code after installation

**"No CMake presets found"**:
- Ensure `CMakePresets.json` exists in project root
- Check `cmake.useCMakePresets` is set to `"always"` in settings

**"IntelliSense errors but build succeeds"**:
- Wait for CMake configuration to complete (watch status bar)
- Check `C_Cpp.default.configurationProvider` is set to `"ms-vscode.cmake-tools"`
- Verify `VCPKG_ROOT` environment variable is set

**"Tasks not showing up"**:
- Ensure `.vscode/tasks.json` exists
- Reload window: Ctrl+Shift+P → "Developer: Reload Window"

## Troubleshooting Build Issues

**CMake can't find dependencies**:
```bash
git submodule update --init --recursive
```

**C++20 not available**:
- Update compiler: gcc 11+, clang 13+, MSVC 19.29+
