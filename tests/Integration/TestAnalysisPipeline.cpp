#include <gtest/gtest.h>
#include <cmath>
#include <numbers>
#include <random>
#include <thread>

#include "Analysis/Fretbuzz/FretBuzzDetector.h"
#include "Analysis/Intonation/IntonationAnalyzer.h"
#include "Analysis/StringHealth/StringHealthAnalyzer.h"
#include "App/AudioProcessingLayer.h"
#include "Util/LockFreeRingBuffer.h"
#include "Analysis/AnalysisEngine.h"

namespace
{

    std::vector<float> GenerateSineWave(float frequency, float sampleRate, size_t numSamples, float amplitude = 1.0f)
    {
        std::vector<float> buffer(numSamples);
        const float twoPi = 2.0f * std::numbers::pi_v<float>;

        for (size_t i = 0; i < numSamples; ++i)
        {
            float phase = twoPi * frequency * static_cast<float>(i) / sampleRate;
            buffer[i] = amplitude * std::sin(phase);
        }

        return buffer;
    }

    std::vector<float> GenerateHarmonicSignal(float fundamental, float sampleRate, size_t numSamples)
    {
        std::vector<float> buffer(numSamples, 0.0f);
        const float twoPi = 2.0f * std::numbers::pi_v<float>;

        for (int harmonic = 1; harmonic <= 5; ++harmonic)
        {
            float amplitude = 1.0f / static_cast<float>(harmonic);
            float freq = fundamental * static_cast<float>(harmonic);

            for (size_t i = 0; i < numSamples; ++i)
            {
                float phase = twoPi * freq * static_cast<float>(i) / sampleRate;
                buffer[i] += amplitude * std::sin(phase);
            }
        }

        float maxVal = *std::max_element(buffer.begin(), buffer.end());
        if (maxVal > 0.0f)
        {
            for (auto &sample : buffer)
            {
                sample /= maxVal;
            }
        }

        return buffer;
    }

    std::vector<float> GenerateWhiteNoise(size_t numSamples, float amplitude = 0.1f)
    {
        std::vector<float> buffer(numSamples);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-amplitude, amplitude);

        for (size_t i = 0; i < numSamples; ++i)
        {
            buffer[i] = dist(gen);
        }

        return buffer;
    }

    std::vector<float> GenerateSilence(size_t numSamples)
    {
        return std::vector<float>(numSamples, 0.0f);
    }

    std::vector<float> GenerateFretBuzzSignal(float fundamental, float sampleRate, size_t numSamples)
    {
        auto clean = GenerateHarmonicSignal(fundamental, sampleRate, numSamples);
        auto noise = GenerateWhiteNoise(numSamples, 0.3f);

        std::vector<float> buffer(numSamples);
        for (size_t i = 0; i < numSamples; ++i)
        {
            buffer[i] = 0.7f * clean[i] + 0.3f * noise[i];
        }

        return buffer;
    }

    std::vector<float> GenerateDecayingHarmonic(float fundamental, float sampleRate, size_t numSamples, float decayRate)
    {
        std::vector<float> buffer(numSamples, 0.0f);
        const float twoPi = 2.0f * std::numbers::pi_v<float>;

        for (int harmonic = 1; harmonic <= 10; ++harmonic)
        {
            float amplitude = 1.0f / static_cast<float>(harmonic);
            float freq = fundamental * static_cast<float>(harmonic);

            for (size_t i = 0; i < numSamples; ++i)
            {
                float time = static_cast<float>(i) / sampleRate;
                float decay = std::exp(-decayRate * time);
                float phase = twoPi * freq * time;
                buffer[i] += amplitude * decay * std::sin(phase);
            }
        }

        float maxVal = *std::max_element(buffer.begin(), buffer.end());
        if (maxVal > 0.0f)
        {
            for (auto &sample : buffer)
            {
                sample /= maxVal;
            }
        }

        return buffer;
    }

} // namespace

class AnalysisPipelineTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sampleRate = 48000.0f;
        bufferSize = 2048;
        ringBufferSize = 16384;

        ringBuffer = std::make_unique<GuitarDiagnostics::Util::LockFreeRingBuffer<float>>(ringBufferSize);

        config = std::make_unique<GuitarDiagnostics::Analysis::AnalysisConfig>(sampleRate, bufferSize);

        engine = std::make_unique<GuitarDiagnostics::Analysis::AnalysisEngine>(ringBuffer.get(), *config);
    }

    void TearDown() override
    {
        if (engine && engine->IsRunning())
        {
            engine->Stop();
        }
        engine.reset();
        ringBuffer.reset();
        config.reset();
    }

    float sampleRate;
    uint32_t bufferSize;
    size_t ringBufferSize;

    std::unique_ptr<GuitarDiagnostics::Util::LockFreeRingBuffer<float>> ringBuffer;
    std::unique_ptr<GuitarDiagnostics::Analysis::AnalysisConfig> config;
    std::unique_ptr<GuitarDiagnostics::Analysis::AnalysisEngine> engine;
};

TEST_F(AnalysisPipelineTest, AllAnalyzersProcessSimultaneously)
{
    auto intonationAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    auto stringHealthAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::StringHealthAnalyzer>();

    engine->RegisterAnalyzer(intonationAnalyzer);
    engine->RegisterAnalyzer(fretBuzzDetector);
    engine->RegisterAnalyzer(stringHealthAnalyzer);

    ASSERT_TRUE(engine->Start());

    auto testSignal = GenerateHarmonicSignal(110.0f, sampleRate, bufferSize);

    for (int i = 0; i < 20; ++i)
    {
        ringBuffer->Write(testSignal);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto intonationResult =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(intonationAnalyzer->GetLatestResult());
    auto fretBuzzResult =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());
    auto stringHealthResult = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(
        stringHealthAnalyzer->GetLatestResult());

    ASSERT_NE(intonationResult, nullptr);
    ASSERT_NE(fretBuzzResult, nullptr);
    ASSERT_NE(stringHealthResult, nullptr);

    EXPECT_TRUE(intonationResult->isValid);
    EXPECT_TRUE(fretBuzzResult->isValid);
    EXPECT_TRUE(stringHealthResult->isValid);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, ThreadSafeResultRetrieval)
{
    auto intonationAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    auto stringHealthAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::StringHealthAnalyzer>();

    engine->RegisterAnalyzer(intonationAnalyzer);
    engine->RegisterAnalyzer(fretBuzzDetector);
    engine->RegisterAnalyzer(stringHealthAnalyzer);

    ASSERT_TRUE(engine->Start());

    std::atomic<bool> running{ true };
    std::atomic<int> readCount{ 0 };

    std::thread producerThread([&]() {
        auto testSignal = GenerateHarmonicSignal(110.0f, sampleRate, bufferSize);
        for (int i = 0; i < 100; ++i)
        {
            ringBuffer->Write(testSignal);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        running = false;
    });

    std::thread consumerThread([&]() {
        while (running)
        {
            auto intonationResult = intonationAnalyzer->GetLatestResult();
            auto fretBuzzResult = fretBuzzDetector->GetLatestResult();
            auto stringHealthResult = stringHealthAnalyzer->GetLatestResult();

            if (intonationResult && fretBuzzResult && stringHealthResult)
            {
                readCount.fetch_add(1, std::memory_order_relaxed);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    producerThread.join();
    consumerThread.join();

    EXPECT_GT(readCount.load(), 0);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, ResetAllAnalyzers)
{
    auto intonationAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    auto stringHealthAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::StringHealthAnalyzer>();

    engine->RegisterAnalyzer(intonationAnalyzer);
    engine->RegisterAnalyzer(fretBuzzDetector);
    engine->RegisterAnalyzer(stringHealthAnalyzer);

    ASSERT_TRUE(engine->Start());

    auto testSignal = GenerateHarmonicSignal(110.0f, sampleRate, bufferSize);

    for (int i = 0; i < 10; ++i)
    {
        ringBuffer->Write(testSignal);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    engine->Reset();

    auto intonationResult =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(intonationAnalyzer->GetLatestResult());

    ASSERT_NE(intonationResult, nullptr);
    EXPECT_EQ(intonationResult->state, GuitarDiagnostics::Analysis::IntonationState::Idle);
    EXPECT_EQ(intonationResult->openStringFrequency, 0.0f);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, ContinuousProcessing)
{
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    engine->RegisterAnalyzer(fretBuzzDetector);

    ASSERT_TRUE(engine->Start());

    for (int frequency = 82; frequency <= 330; frequency += 82)
    {
        auto testSignal = GenerateSineWave(static_cast<float>(frequency), sampleRate, bufferSize);

        for (int i = 0; i < 5; ++i)
        {
            ringBuffer->Write(testSignal);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, EngineStartStop)
{
    auto analyzer = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    engine->RegisterAnalyzer(analyzer);

    ASSERT_TRUE(engine->Start());
    EXPECT_TRUE(engine->IsRunning());

    engine->Stop();
    EXPECT_FALSE(engine->IsRunning());

    ASSERT_TRUE(engine->Start());
    EXPECT_TRUE(engine->IsRunning());

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, EmptyBufferHandling)
{
    auto analyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();
    engine->RegisterAnalyzer(analyzer);

    ASSERT_TRUE(engine->Start());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto result = analyzer->GetLatestResult();
    ASSERT_NE(result, nullptr);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, AudioProcessingLayerInitialization)
{
    auto audioLayer = std::make_unique<GuitarDiagnostics::App::AudioProcessingLayer>(ringBuffer.get());

    EXPECT_FALSE(audioLayer->IsOpen());
    EXPECT_FALSE(audioLayer->IsRunning());
}

TEST_F(AnalysisPipelineTest, HighFrequencyWriteStressTest)
{
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    auto intonationAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();
    auto stringHealthAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::StringHealthAnalyzer>();

    engine->RegisterAnalyzer(fretBuzzDetector);
    engine->RegisterAnalyzer(intonationAnalyzer);
    engine->RegisterAnalyzer(stringHealthAnalyzer);

    ASSERT_TRUE(engine->Start());

    auto testSignal = GenerateHarmonicSignal(110.0f, sampleRate, bufferSize);

    for (int i = 0; i < 100; ++i)
    {
        bool writeSuccess = ringBuffer->Write(testSignal);
        EXPECT_TRUE(writeSuccess);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto fretBuzzResult =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());
    auto intonationResult =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(intonationAnalyzer->GetLatestResult());
    auto stringHealthResult = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(
        stringHealthAnalyzer->GetLatestResult());

    ASSERT_NE(fretBuzzResult, nullptr);
    ASSERT_NE(intonationResult, nullptr);
    ASSERT_NE(stringHealthResult, nullptr);

    EXPECT_TRUE(fretBuzzResult->isValid);
    EXPECT_TRUE(intonationResult->isValid);
    EXPECT_TRUE(stringHealthResult->isValid);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, SilenceHandling)
{
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    engine->RegisterAnalyzer(fretBuzzDetector);

    ASSERT_TRUE(engine->Start());

    auto silence = GenerateSilence(bufferSize);

    for (int i = 0; i < 10; ++i)
    {
        ringBuffer->Write(silence);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, FretBuzzDetectionWithNoiseSignal)
{
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    engine->RegisterAnalyzer(fretBuzzDetector);

    ASSERT_TRUE(engine->Start());

    auto buzzSignal = GenerateFretBuzzSignal(110.0f, sampleRate, bufferSize);

    for (int i = 0; i < 20; ++i)
    {
        ringBuffer->Write(buzzSignal);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
    EXPECT_GE(result->buzzScore, 0.0f);
    EXPECT_LE(result->buzzScore, 1.0f);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, StringHealthWithDecayingSignal)
{
    auto stringHealthAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::StringHealthAnalyzer>();
    engine->RegisterAnalyzer(stringHealthAnalyzer);

    ASSERT_TRUE(engine->Start());

    auto decayingSignal = GenerateDecayingHarmonic(110.0f, sampleRate, bufferSize, 2.0f);

    for (int i = 0; i < 30; ++i)
    {
        ringBuffer->Write(decayingSignal);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(
        stringHealthAnalyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
    EXPECT_GE(result->healthScore, 0.0f);
    EXPECT_LE(result->healthScore, 1.0f);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, IntonationStateTransitions)
{
    auto intonationAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();
    engine->RegisterAnalyzer(intonationAnalyzer);

    ASSERT_TRUE(engine->Start());

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(intonationAnalyzer->GetLatestResult());
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->state, GuitarDiagnostics::Analysis::IntonationState::Idle);

    auto openStringSignal = GenerateHarmonicSignal(82.41f, sampleRate, bufferSize);

    for (int i = 0; i < 30; ++i)
    {
        ringBuffer->Write(openStringSignal);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(intonationAnalyzer->GetLatestResult());
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->state == GuitarDiagnostics::Analysis::IntonationState::OpenString
                || result->state == GuitarDiagnostics::Analysis::IntonationState::WaitFor12thFret);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, RapidFrequencyChanges)
{
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    engine->RegisterAnalyzer(fretBuzzDetector);

    ASSERT_TRUE(engine->Start());

    float frequencies[] = { 82.41f, 110.0f, 146.83f, 196.0f, 246.94f, 329.63f };
    const int numFrequencies = sizeof(frequencies) / sizeof(frequencies[0]);

    for (int i = 0; i < numFrequencies; ++i)
    {
        auto signal = GenerateHarmonicSignal(frequencies[i], sampleRate, bufferSize);

        for (int j = 0; j < 5; ++j)
        {
            ringBuffer->Write(signal);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);

    engine->Stop();
}

TEST_F(AnalysisPipelineTest, BufferOverflowPrevention)
{
    auto analyzer = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    engine->RegisterAnalyzer(analyzer);

    auto testSignal = GenerateHarmonicSignal(110.0f, sampleRate, bufferSize);

    bool overflowDetected = false;
    for (int i = 0; i < 100; ++i)
    {
        bool writeSuccess = ringBuffer->Write(testSignal);
        if (!writeSuccess)
        {
            overflowDetected = true;
            break;
        }
    }

    EXPECT_TRUE(overflowDetected);
}

TEST_F(AnalysisPipelineTest, MultipleResetCycles)
{
    auto fretBuzzDetector = std::make_shared<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    auto intonationAnalyzer = std::make_shared<GuitarDiagnostics::Analysis::IntonationAnalyzer>();

    engine->RegisterAnalyzer(fretBuzzDetector);
    engine->RegisterAnalyzer(intonationAnalyzer);

    ASSERT_TRUE(engine->Start());

    for (int cycle = 0; cycle < 3; ++cycle)
    {
        auto testSignal = GenerateHarmonicSignal(110.0f, sampleRate, bufferSize);

        for (int i = 0; i < 10; ++i)
        {
            ringBuffer->Write(testSignal);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        auto fretBuzzResult =
            std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(fretBuzzDetector->GetLatestResult());
        auto intonationResult = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(
            intonationAnalyzer->GetLatestResult());

        ASSERT_NE(fretBuzzResult, nullptr);
        ASSERT_NE(intonationResult, nullptr);
        EXPECT_TRUE(fretBuzzResult->isValid);
        EXPECT_TRUE(intonationResult->isValid);

        engine->Reset();

        intonationResult = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::IntonationResult>(
            intonationAnalyzer->GetLatestResult());
        ASSERT_NE(intonationResult, nullptr);
        EXPECT_EQ(intonationResult->state, GuitarDiagnostics::Analysis::IntonationState::Idle);
    }

    engine->Stop();
}
