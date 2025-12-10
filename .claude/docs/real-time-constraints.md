# Real-Time Audio Constraints

## Audio Callback Safety Rules

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

## Pitfall #3: Allocations in Audio Callback

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

## Troubleshooting Runtime Issues

**Audio clicks/pops**:
- Check for allocations in audio callback
- Reduce buffer size (512 → 256 samples)
- Increase thread priority

**High CPU usage**:
- Profile with tools (perf, Instruments, VTune)
- Optimize FFT size
- Reduce analysis rate
