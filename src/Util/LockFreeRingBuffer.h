#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>
#include <span>

namespace GuitarDiagnostics::Util
{

    /**
     * @brief A single-producer, single-consumer lock-free ring buffer.
     *
     * @tparam T The type of elements stored in the buffer.
     */
    template<typename T> class LockFreeRingBuffer
    {
    public:
        /**
         * @brief Constructs the ring buffer with a specific capacity.
         * @param capacity The maximum number of elements the buffer can hold.
         */
        explicit LockFreeRingBuffer(size_t capacity);

        /**
         * @brief Destructor.
         */
        ~LockFreeRingBuffer() = default;

        LockFreeRingBuffer(const LockFreeRingBuffer &) = delete;

        LockFreeRingBuffer &operator=(const LockFreeRingBuffer &) = delete;

        LockFreeRingBuffer(LockFreeRingBuffer &&) noexcept = delete;

        LockFreeRingBuffer &operator=(LockFreeRingBuffer &&) noexcept = delete;

        /**
         * @brief Writes data to the buffer.
         * @param data The data span to write.
         * @return True if all data was written, false if not enough space.
         */
        bool Write(std::span<const T> data) noexcept;

        /**
         * @brief Reads data from the buffer.
         * @param output The span to write read data into.
         * @return The number of elements actually read.
         */
        size_t Read(std::span<T> output) noexcept;

        /**
         * @brief Gets the number of elements available to read.
         * @return Number of available elements.
         */
        size_t GetAvailableRead() const noexcept;

    private:
        /**
         * @brief Internal helper to calculate available read items.
         * @param readIdx Current read index.
         * @param writeIdx Current write index.
         * @return Number of available items.
         */
        size_t GetAvailableReadInternal(size_t readIdx, size_t writeIdx) const noexcept;

        /**
         * @brief Internal helper to calculate available write items.
         * @param writeIdx Current write index.
         * @param readIdx Current read index.
         * @return Number of available slots.
         */
        size_t GetAvailableWriteInternal(size_t writeIdx, size_t readIdx) const noexcept;

        const size_t capacity;          ///< Maximum capacity of the buffer.
        std::unique_ptr<T[]> buffer;    ///< Storage buffer.
        std::atomic<size_t> writeIndex; ///< Write definition index (atomic).
        std::atomic<size_t> readIndex;  ///< Read definition index (atomic).
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
