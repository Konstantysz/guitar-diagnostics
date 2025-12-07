#include "Util/LockFreeRingBuffer.h"
#include <gtest/gtest.h>
#include <array>
#include <thread>
#include <vector>

class LockFreeRingBufferTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        buffer = std::make_unique<GuitarDiagnostics::Util::LockFreeRingBuffer<float>>(1024);
    }

    void TearDown() override
    {
        buffer.reset();
    }

    std::unique_ptr<GuitarDiagnostics::Util::LockFreeRingBuffer<float>> buffer;
};

// Test 1: Write and read single element
TEST_F(LockFreeRingBufferTest, WriteAndReadSingleElement)
{
    std::array<float, 1> input = { 42.0f };
    ASSERT_TRUE(buffer->Write(input));

    std::array<float, 1> output = { 0.0f };
    ASSERT_EQ(buffer->Read(output), 1);
    EXPECT_FLOAT_EQ(output[0], 42.0f);
}

// Test 2: Write and read multiple elements
TEST_F(LockFreeRingBufferTest, WriteAndReadMultipleElements)
{
    std::array<float, 5> input = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    ASSERT_TRUE(buffer->Write(input));

    std::array<float, 5> output = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    ASSERT_EQ(buffer->Read(output), 5);

    for (size_t i = 0; i < 5; ++i)
    {
        EXPECT_FLOAT_EQ(output[i], input[i]);
    }
}

// Test 3: Read from empty buffer returns 0
TEST_F(LockFreeRingBufferTest, ReadFromEmptyBuffer)
{
    std::array<float, 10> output;
    EXPECT_EQ(buffer->Read(output), 0);
}

// Test 4: Write to full buffer returns false
TEST_F(LockFreeRingBufferTest, WriteToFullBuffer)
{
    std::vector<float> largeInput(1024, 1.0f);
    ASSERT_TRUE(buffer->Write(largeInput));

    std::array<float, 1> extraInput = { 99.0f };
    EXPECT_FALSE(buffer->Write(extraInput));
}

// Test 5: Wrap around buffer
TEST_F(LockFreeRingBufferTest, WrapAroundBuffer)
{
    std::vector<float> input1(512, 1.0f);
    ASSERT_TRUE(buffer->Write(input1));

    std::vector<float> output1(512);
    ASSERT_EQ(buffer->Read(output1), 512);

    std::vector<float> input2(600, 2.0f);
    ASSERT_TRUE(buffer->Write(input2));

    std::vector<float> output2(600);
    ASSERT_EQ(buffer->Read(output2), 600);

    for (size_t i = 0; i < 600; ++i)
    {
        EXPECT_FLOAT_EQ(output2[i], 2.0f);
    }
}

// Test 6: Get available read space
TEST_F(LockFreeRingBufferTest, GetAvailableRead)
{
    EXPECT_EQ(buffer->GetAvailableRead(), 0);

    std::array<float, 100> input;
    buffer->Write(input);

    EXPECT_EQ(buffer->GetAvailableRead(), 100);

    std::array<float, 50> output;
    buffer->Read(output);

    EXPECT_EQ(buffer->GetAvailableRead(), 50);
}

// Test 7: Partial read when not enough data available
TEST_F(LockFreeRingBufferTest, PartialRead)
{
    std::array<float, 10> input = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f };
    ASSERT_TRUE(buffer->Write(input));

    std::array<float, 20> output;
    size_t readCount = buffer->Read(output);

    EXPECT_EQ(readCount, 10);
    for (size_t i = 0; i < 10; ++i)
    {
        EXPECT_FLOAT_EQ(output[i], input[i]);
    }
}

// Test 8: Thread safety - concurrent write and read
TEST_F(LockFreeRingBufferTest, ConcurrentWriteAndRead)
{
    constexpr size_t numSamples = 10000;
    std::atomic<bool> writerDone{ false };
    std::atomic<size_t> totalRead{ 0 };

    std::thread writer([this, &writerDone]() {
        for (size_t i = 0; i < numSamples; ++i)
        {
            std::array<float, 1> data = { static_cast<float>(i) };
            while (!buffer->Write(data))
            {
                std::this_thread::yield();
            }
        }
        writerDone = true;
    });

    std::thread reader([this, &writerDone, &totalRead]() {
        std::vector<float> readData;
        readData.reserve(numSamples);

        while (!writerDone || buffer->GetAvailableRead() > 0)
        {
            std::array<float, 1> data;
            if (buffer->Read(data) == 1)
            {
                readData.push_back(data[0]);
            }
            else
            {
                std::this_thread::yield();
            }
        }

        totalRead = readData.size();

        for (size_t i = 0; i < readData.size(); ++i)
        {
            EXPECT_FLOAT_EQ(readData[i], static_cast<float>(i));
        }
    });

    writer.join();
    reader.join();

    EXPECT_EQ(totalRead, numSamples);
}

// Test 9: Zero-sized write/read
TEST_F(LockFreeRingBufferTest, ZeroSizedOperations)
{
    std::span<const float> emptyInput;
    EXPECT_TRUE(buffer->Write(emptyInput));

    std::span<float> emptyOutput;
    EXPECT_EQ(buffer->Read(emptyOutput), 0);
}

// Test 10: Multiple writes before read
TEST_F(LockFreeRingBufferTest, MultipleWritesBeforeRead)
{
    std::array<float, 10> input1;
    std::array<float, 20> input2;
    std::array<float, 30> input3;

    for (size_t i = 0; i < 10; ++i)
        input1[i] = static_cast<float>(i);
    for (size_t i = 0; i < 20; ++i)
        input2[i] = static_cast<float>(i + 10);
    for (size_t i = 0; i < 30; ++i)
        input3[i] = static_cast<float>(i + 30);

    ASSERT_TRUE(buffer->Write(input1));
    ASSERT_TRUE(buffer->Write(input2));
    ASSERT_TRUE(buffer->Write(input3));

    EXPECT_EQ(buffer->GetAvailableRead(), 60);

    std::vector<float> output(60);
    ASSERT_EQ(buffer->Read(output), 60);

    for (size_t i = 0; i < 60; ++i)
    {
        EXPECT_FLOAT_EQ(output[i], static_cast<float>(i));
    }
}
