#include <gtest/gtest.h>

#include "Util/LockFreeRingBuffer.h"
#include "Analysis/AnalysisEngine.h"

#include <array>
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

using namespace GuitarDiagnostics::Analysis;
using namespace GuitarDiagnostics::Util;

namespace
{

    class CountingAnalyzer : public Analyzer
    {
    public:
        std::atomic<int> processCount;

        CountingAnalyzer() : processCount(0)
        {
        }

        void Configure(const AnalysisConfig &) override
        {
        }

        void ProcessBuffer(std::span<const float>) override
        {
            processCount.fetch_add(1, std::memory_order_relaxed);
        }

        std::shared_ptr<AnalysisResult> GetLatestResult() const override
        {
            return std::make_shared<AnalysisResult>();
        }

        void Reset() override
        {
            processCount.store(0, std::memory_order_relaxed);
        }
    };

} // anonymous namespace

class AnalysisEngineTest : public ::testing::Test
{
protected:
    std::unique_ptr<AnalysisEngine> engine;
    std::unique_ptr<LockFreeRingBuffer<float>> ringBuffer;
    AnalysisConfig config;

    AnalysisEngineTest() : engine(nullptr), ringBuffer(nullptr), config(48000.0f, 512)
    {
    }

    void SetUp() override
    {
        ringBuffer = std::make_unique<LockFreeRingBuffer<float>>(4096);
    }

    void TearDown() override
    {
        if (engine)
        {
            engine->Stop();
            engine.reset();
        }
        ringBuffer.reset();
    }
};

TEST_F(AnalysisEngineTest, CreateInstance)
{
    ASSERT_NO_THROW(engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config));
    ASSERT_NE(engine, nullptr);
}

TEST_F(AnalysisEngineTest, StartAndStop)
{
    engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config);

    ASSERT_TRUE(engine->Start());
    EXPECT_TRUE(engine->IsRunning());

    engine->Stop();
    EXPECT_FALSE(engine->IsRunning());
}

TEST_F(AnalysisEngineTest, DoubleStart)
{
    engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config);

    ASSERT_TRUE(engine->Start());
    EXPECT_TRUE(engine->IsRunning());

    // Second start should fail or be no-op
    EXPECT_FALSE(engine->Start());
    EXPECT_TRUE(engine->IsRunning());

    engine->Stop();
}

TEST_F(AnalysisEngineTest, DoubleStop)
{
    engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config);

    ASSERT_TRUE(engine->Start());
    EXPECT_TRUE(engine->IsRunning());

    engine->Stop();
    EXPECT_FALSE(engine->IsRunning());

    // Second stop should be safe
    ASSERT_NO_THROW(engine->Stop());
    EXPECT_FALSE(engine->IsRunning());
}

TEST_F(AnalysisEngineTest, ProcessAudioData)
{
    engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config);
    ASSERT_TRUE(engine->Start());

    // Write test audio data
    std::array<float, 512> testData;
    for (size_t i = 0; i < testData.size(); ++i)
    {
        testData[i] = static_cast<float>(i) / 512.0f;
    }

    ASSERT_TRUE(ringBuffer->Write(testData));

    // Give engine time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    engine->Stop();
}

TEST_F(AnalysisEngineTest, RegisterAnalyzer)
{
    engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config);

    // Create mock analyzer
    class TestAnalyzer : public Analyzer
    {
    public:
        bool configured = false;
        bool processed = false;

        void Configure(const AnalysisConfig &) override
        {
            configured = true;
        }

        void ProcessBuffer(std::span<const float>) override
        {
            processed = true;
        }

        std::shared_ptr<AnalysisResult> GetLatestResult() const override
        {
            return std::make_shared<AnalysisResult>();
        }

        void Reset() override
        {
            processed = false;
        }
    };

    auto analyzer = std::make_shared<TestAnalyzer>();
    engine->RegisterAnalyzer(analyzer);

    // Analyzer should be configured immediately
    EXPECT_TRUE(analyzer->configured);

    engine->Start();

    // Write test data
    std::array<float, 512> testData;
    testData.fill(0.5f);
    ringBuffer->Write(testData);

    // Give time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Analyzer should have processed data
    EXPECT_TRUE(analyzer->processed);

    engine->Stop();
}

TEST_F(AnalysisEngineTest, MultipleAnalyzers)
{
    engine = std::make_unique<AnalysisEngine>(ringBuffer.get(), config);

    auto analyzer1 = std::make_shared<CountingAnalyzer>();
    auto analyzer2 = std::make_shared<CountingAnalyzer>();
    auto analyzer3 = std::make_shared<CountingAnalyzer>();

    engine->RegisterAnalyzer(analyzer1);
    engine->RegisterAnalyzer(analyzer2);
    engine->RegisterAnalyzer(analyzer3);

    engine->Start();

    // Write test data
    std::array<float, 512> testData;
    testData.fill(0.5f);
    ringBuffer->Write(testData);

    // Give time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // All analyzers should have processed
    EXPECT_GT(analyzer1->processCount.load(), 0);
    EXPECT_GT(analyzer2->processCount.load(), 0);
    EXPECT_GT(analyzer3->processCount.load(), 0);

    engine->Stop();
}
