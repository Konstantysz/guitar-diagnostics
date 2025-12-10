# C++ Coding Standards

## File Organization

- **Headers**: All headers (.h files) are placed in `src/` directory, NOT in `include/`
- **Header Guards**: Use `#pragma once` (NOT `#ifndef` guards)
- **Directory Structure**: Headers in category folders like `src/Analysis/`, `src/UI/`, `src/Util/`

## Naming Conventions

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

## Namespace Rules

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

## Critical Rules

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

## Code Formatting

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

## Modern C++20 Standards

**Prioritize C++20 features** for safer, more expressive code:

*   **`std::span`**: ALWAYS use `std::span<T>` instead of `T*` + size pairs for array views.
*   **Concepts**: Prefer C++20 Concepts (`requires`) over `std::enable_if` for template constraints.
*   **`consteval` / `constinit`**: Maximize compile-time computation and initialization.
*   **`<ranges>`**: Use ranges for complex data transformations (unless performance profiling shows issues in hot paths).
*   **Designated Initializers**: Use for structs (e.g., `QueueParams p = { .size = 1024 };`).

## Safety & Best Practices

*   **Casting**:
    *   ❌ **BAN** C-style casts `(int)x`.
    *   ✅ **USE** `static_cast`, `reinterpret_cast`, etc.
*   **Pointers**:
    *   ✅ **ALWAYS** use `nullptr`, never `NULL` or `0`.
    *   ✅ **USE** `std::unique_ptr` for exclusive ownership.
    *   ✅ **USE** Raw pointers (`T*`) **ONLY** for non-owning views/observers.
*   **Constructors**: Mark single-argument constructors as `explicit` to prevent accidental implicit conversions.
*   **Const Correctness**:
    *   Mark member functions `const` if they don't modify logical state.
    *   Pass non-primitive objects by `const&`.

## Documentation Standards

*   **Member Variables**: Use `///< Description` (trailing style).
*   **Methods**: Use `/** ... */` block before the method.
*   **Required Tags**: `@param`, `@return` for non-trivial public methods.
*   **Content**: Focus on thread-safety guarantees and *why* the method exists.

## Include Hygiene

**Order**:
1.  Corresponding Header (e.g., `MyClass.h` in `MyClass.cpp`)
2.  Standard Library (`<vector>`, `<span>`, `<memory>`)
3.  Third-Party Libraries (`<imgui.h>`, `<spdlog/spdlog.h>`)
4.  Project Headers (`"Util/LockFreeRingBuffer.h"`)

## Design Principles

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
