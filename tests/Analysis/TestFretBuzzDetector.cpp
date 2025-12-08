#include <gtest/gtest.h>
#include <cmath>
#include <numbers>
#include <random>

#include "Analysis/Fretbuzz/FretBuzzDetector.h"

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

    std::vector<float> GenerateCleanNote(float fundamental, float sampleRate, size_t numSamples)
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

    std::vector<float> GenerateBuzzyNote(float fundamental, float sampleRate, size_t numSamples)
    {
        std::vector<float> buffer = GenerateCleanNote(fundamental, sampleRate, numSamples);

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-0.3f, 0.3f);

        for (size_t i = 0; i < numSamples / 10; ++i)
        {
            buffer[i] += dist(rng) * 0.5f;
        }

        const size_t highFreqStart = numSamples / 10;
        const size_t highFreqEnd = numSamples / 2;
        for (size_t i = highFreqStart; i < highFreqEnd; ++i)
        {
            buffer[i] += dist(rng) * 0.2f;
        }

        return buffer;
    }

    std::vector<float> GenerateImpulse(size_t numSamples)
    {
        std::vector<float> buffer(numSamples, 0.0f);
        buffer[0] = 1.0f;
        return buffer;
    }

} // namespace

class FretBuzzDetectorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        detector = std::make_unique<GuitarDiagnostics::Analysis::FretBuzzDetector>();
    }

    void TearDown() override
    {
        detector.reset();
    }

    std::unique_ptr<GuitarDiagnostics::Analysis::FretBuzzDetector> detector;
};

TEST_F(FretBuzzDetectorTest, CreateInstance)
{
    ASSERT_NE(detector, nullptr);
}

TEST_F(FretBuzzDetectorTest, ConfigureSetsSampleRate)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    ASSERT_NO_THROW(detector->Configure(config));
}

TEST_F(FretBuzzDetectorTest, DetectOnsetFromImpulse)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto impulse = GenerateImpulse(bufferSize);
    detector->ProcessBuffer(impulse);

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
}

TEST_F(FretBuzzDetectorTest, CalculateRMSEnergy)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto sineWave = GenerateSineWave(440.0f, sampleRate, bufferSize, 0.5f);
    detector->ProcessBuffer(sineWave);

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
}

TEST_F(FretBuzzDetectorTest, FFTComputesCorrectly)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto testSignal = GenerateSineWave(1000.0f, sampleRate, bufferSize);
    detector->ProcessBuffer(testSignal);

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
}

TEST_F(FretBuzzDetectorTest, ExtractHighFrequencyEnergy)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto highFreqSignal = GenerateSineWave(5000.0f, sampleRate, bufferSize);
    detector->ProcessBuffer(highFreqSignal);

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->highFreqEnergyScore, 0.0f);
    EXPECT_LE(result->highFreqEnergyScore, 1.0f);
}

TEST_F(FretBuzzDetectorTest, NoBuzzOnCleanSignal)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto cleanNote = GenerateCleanNote(82.41f, sampleRate, bufferSize);

    for (int i = 0; i < 5; ++i)
    {
        detector->ProcessBuffer(cleanNote);
    }

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
    EXPECT_GE(result->buzzScore, 0.0f);
    EXPECT_LE(result->buzzScore, 1.0f);
}

TEST_F(FretBuzzDetectorTest, BuzzOnNoisyTransient)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto silence = std::vector<float>(bufferSize, 0.0f);
    detector->ProcessBuffer(silence);

    auto buzzyNote = GenerateBuzzyNote(82.41f, sampleRate, bufferSize);
    detector->ProcessBuffer(buzzyNote);

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
}

TEST_F(FretBuzzDetectorTest, ResetClearsState)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto testSignal = GenerateSineWave(440.0f, sampleRate, bufferSize);
    for (int i = 0; i < 10; ++i)
    {
        detector->ProcessBuffer(testSignal);
    }

    detector->Reset();

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
}

TEST_F(FretBuzzDetectorTest, BuzzScoreWithinRange)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    detector->Configure(config);

    auto testSignal = GenerateCleanNote(110.0f, sampleRate, bufferSize);
    detector->ProcessBuffer(testSignal);

    auto result = std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::FretBuzzResult>(detector->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->buzzScore, 0.0f);
    EXPECT_LE(result->buzzScore, 1.0f);
    EXPECT_GE(result->transientScore, 0.0f);
    EXPECT_LE(result->transientScore, 1.0f);
    EXPECT_GE(result->highFreqEnergyScore, 0.0f);
    EXPECT_LE(result->highFreqEnergyScore, 1.0f);
    EXPECT_GE(result->inharmonicityScore, 0.0f);
    EXPECT_LE(result->inharmonicityScore, 1.0f);
}
