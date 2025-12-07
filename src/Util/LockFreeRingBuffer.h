#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>
#include <span>

namespace GuitarDiagnostics::Util
{

    template<typename T> class LockFreeRingBuffer
    {
    public:
        explicit LockFreeRingBuffer(size_t capacity);
        ~LockFreeRingBuffer() = default;

        LockFreeRingBuffer(const LockFreeRingBuffer &) = delete;
        LockFreeRingBuffer &operator=(const LockFreeRingBuffer &) = delete;
        LockFreeRingBuffer(LockFreeRingBuffer &&) noexcept = delete;
        LockFreeRingBuffer &operator=(LockFreeRingBuffer &&) noexcept = delete;

        bool Write(std::span<const T> data) noexcept;
        size_t Read(std::span<T> output) noexcept;
        size_t GetAvailableRead() const noexcept;

    private:
        size_t GetAvailableReadInternal(size_t readIdx, size_t writeIdx) const noexcept;
        size_t GetAvailableWriteInternal(size_t writeIdx, size_t readIdx) const noexcept;

        const size_t capacity;
        std::unique_ptr<T[]> buffer;
        std::atomic<size_t> writeIndex;
        std::atomic<size_t> readIndex;
    };

    template<typename T>
    LockFreeRingBuffer<T>::LockFreeRingBuffer(size_t capacity)
        : capacity(capacity + 1), buffer(std::make_unique<T[]>(capacity + 1)), writeIndex(0), readIndex(0)
    {
    }

    template<typename T> bool LockFreeRingBuffer<T>::Write(std::span<const T> data) noexcept
    {
        if (data.empty())
        {
            return true;
        }

        const size_t writeIdx = writeIndex.load(std::memory_order_relaxed);
        const size_t readIdx = readIndex.load(std::memory_order_acquire);

        const size_t available = GetAvailableWriteInternal(writeIdx, readIdx);
        if (available < data.size())
        {
            return false;
        }

        for (size_t i = 0; i < data.size(); ++i)
        {
            buffer[(writeIdx + i) % capacity] = data[i];
        }

        writeIndex.store((writeIdx + data.size()) % capacity, std::memory_order_release);

        return true;
    }

    template<typename T> size_t LockFreeRingBuffer<T>::Read(std::span<T> output) noexcept
    {
        if (output.empty())
        {
            return 0;
        }

        const size_t readIdx = readIndex.load(std::memory_order_relaxed);
        const size_t writeIdx = writeIndex.load(std::memory_order_acquire);

        const size_t available = GetAvailableReadInternal(readIdx, writeIdx);
        const size_t toRead = std::min(available, output.size());

        for (size_t i = 0; i < toRead; ++i)
        {
            output[i] = buffer[(readIdx + i) % capacity];
        }

        readIndex.store((readIdx + toRead) % capacity, std::memory_order_release);

        return toRead;
    }

    template<typename T> size_t LockFreeRingBuffer<T>::GetAvailableRead() const noexcept
    {
        const size_t readIdx = readIndex.load(std::memory_order_relaxed);
        const size_t writeIdx = writeIndex.load(std::memory_order_acquire);
        return GetAvailableReadInternal(readIdx, writeIdx);
    }

    template<typename T>
    size_t LockFreeRingBuffer<T>::GetAvailableReadInternal(size_t readIdx, size_t writeIdx) const noexcept
    {
        if (writeIdx >= readIdx)
        {
            return writeIdx - readIdx;
        }
        return capacity - readIdx + writeIdx;
    }

    template<typename T>
    size_t LockFreeRingBuffer<T>::GetAvailableWriteInternal(size_t writeIdx, size_t readIdx) const noexcept
    {
        return capacity - 1 - GetAvailableReadInternal(readIdx, writeIdx);
    }

} // namespace GuitarDiagnostics::Util
