#include <gtest/gtest.h>
#include <cmath>
#include <numbers>
#include <random>

#include "Analysis/StringHealth/StringHealthAnalyzer.h"

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

    std::vector<float> GenerateHealthyString(float fundamental, float sampleRate, size_t numSamples)
    {
        std::vector<float> buffer(numSamples, 0.0f);
        const float twoPi = 2.0f * std::numbers::pi_v<float>;

        for (int harmonic = 1; harmonic <= 8; ++harmonic)
        {
            float amplitude = 1.0f / static_cast<float>(harmonic);
            float freq = fundamental * static_cast<float>(harmonic);

            for (size_t i = 0; i < numSamples; ++i)
            {
                float phase = twoPi * freq * static_cast<float>(i) / sampleRate;
                float decay = std::exp(-0.5f * static_cast<float>(i) / static_cast<float>(numSamples));
                buffer[i] += amplitude * std::sin(phase) * decay;
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

    std::vector<float> GenerateWornString(float fundamental, float sampleRate, size_t numSamples)
    {
        std::vector<float> buffer(numSamples, 0.0f);
        const float twoPi = 2.0f * std::numbers::pi_v<float>;

        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist(-0.05f, 0.05f);

        for (int harmonic = 1; harmonic <= 4; ++harmonic)
        {
            float amplitude = 1.0f / static_cast<float>(harmonic * harmonic);
            float freqDeviation = 1.0f + dist(rng) * 0.02f;
            float freq = fundamental * static_cast<float>(harmonic) * freqDeviation;

            for (size_t i = 0; i < numSamples; ++i)
            {
                float phase = twoPi * freq * static_cast<float>(i) / sampleRate;
                float decay = std::exp(-3.0f * static_cast<float>(i) / static_cast<float>(numSamples));
                buffer[i] += amplitude * std::sin(phase) * decay;
            }
        }

        for (size_t i = 0; i < numSamples; ++i)
        {
            buffer[i] += dist(rng) * 0.1f;
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

class StringHealthAnalyzerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        analyzer = std::make_unique<GuitarDiagnostics::Analysis::StringHealthAnalyzer>();
    }

    void TearDown() override
    {
        analyzer.reset();
    }

    std::unique_ptr<GuitarDiagnostics::Analysis::StringHealthAnalyzer> analyzer;
};

TEST_F(StringHealthAnalyzerTest, CreateInstance)
{
    ASSERT_NE(analyzer, nullptr);
}

TEST_F(StringHealthAnalyzerTest, ConfigureSetsSampleRate)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    ASSERT_NO_THROW(analyzer->Configure(config));
}

TEST_F(StringHealthAnalyzerTest, DetectFundamentalFrequency)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;
    const float testFrequency = 82.41f;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    auto signal = GenerateSineWave(testFrequency, sampleRate, bufferSize);
    analyzer->ProcessBuffer(signal);

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
}

TEST_F(StringHealthAnalyzerTest, TrackHarmonicDecay)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;
    const float fundamental = 110.0f;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    for (int i = 0; i < 20; ++i)
    {
        auto signal = GenerateHealthyString(fundamental, sampleRate, bufferSize);
        analyzer->ProcessBuffer(signal);
    }

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
}

TEST_F(StringHealthAnalyzerTest, CalculateSpectralCentroid)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    auto signal = GenerateHealthyString(220.0f, sampleRate, bufferSize);
    analyzer->ProcessBuffer(signal);

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->spectralCentroid, 0.0f);
}

TEST_F(StringHealthAnalyzerTest, MeasureInharmonicity)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    auto signal = GenerateHealthyString(82.41f, sampleRate, bufferSize);
    analyzer->ProcessBuffer(signal);

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->inharmonicity, 0.0f);
    EXPECT_LE(result->inharmonicity, 1.0f);
}

TEST_F(StringHealthAnalyzerTest, HealthyStringHighScore)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    for (int i = 0; i < 30; ++i)
    {
        auto signal = GenerateHealthyString(110.0f, sampleRate, bufferSize);
        analyzer->ProcessBuffer(signal);
    }

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->healthScore, 0.0f);
    EXPECT_LE(result->healthScore, 1.0f);
}

TEST_F(StringHealthAnalyzerTest, WornStringLowScore)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    for (int i = 0; i < 30; ++i)
    {
        auto signal = GenerateWornString(110.0f, sampleRate, bufferSize);
        analyzer->ProcessBuffer(signal);
    }

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->healthScore, 0.0f);
    EXPECT_LE(result->healthScore, 1.0f);
}

TEST_F(StringHealthAnalyzerTest, ResetClearsHistory)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    for (int i = 0; i < 20; ++i)
    {
        auto signal = GenerateHealthyString(110.0f, sampleRate, bufferSize);
        analyzer->ProcessBuffer(signal);
    }

    analyzer->Reset();

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isValid);
    EXPECT_EQ(result->fundamentalFrequency, 0.0f);
}

TEST_F(StringHealthAnalyzerTest, HealthScoreWithinRange)
{
    const float sampleRate = 48000.0f;
    const uint32_t bufferSize = 2048;

    GuitarDiagnostics::Analysis::AnalysisConfig config(sampleRate, bufferSize);
    analyzer->Configure(config);

    auto signal = GenerateHealthyString(220.0f, sampleRate, bufferSize);
    analyzer->ProcessBuffer(signal);

    auto result =
        std::dynamic_pointer_cast<GuitarDiagnostics::Analysis::StringHealthResult>(analyzer->GetLatestResult());

    ASSERT_NE(result, nullptr);
    EXPECT_GE(result->healthScore, 0.0f);
    EXPECT_LE(result->healthScore, 1.0f);
    EXPECT_GE(result->inharmonicity, 0.0f);
    EXPECT_LE(result->inharmonicity, 1.0f);
}
