#pragma once

#include "Analysis/Analyzer.h"

#include <FFTProcessor.h>
#include <YinPitchDetector.h>

#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    struct StringHealthResult : public AnalysisResult
    {
        float healthScore;
        float decayRate;
        float spectralCentroid;
        float inharmonicity;
        float fundamentalFrequency;

        StringHealthResult();
    };

    class StringHealthAnalyzer : public Analyzer
    {
    public:
        StringHealthAnalyzer();
        ~StringHealthAnalyzer() override;

        StringHealthAnalyzer(const StringHealthAnalyzer &) = delete;
        StringHealthAnalyzer &operator=(const StringHealthAnalyzer &) = delete;
        StringHealthAnalyzer(StringHealthAnalyzer &&) = delete;
        StringHealthAnalyzer &operator=(StringHealthAnalyzer &&) = delete;

        void Configure(const AnalysisConfig &config) override;
        void ProcessBuffer(std::span<const float> audioData) override;
        std::shared_ptr<AnalysisResult> GetLatestResult() const override;
        void Reset() override;

    private:
        float AnalyzeDecay();
        void TrackHarmonicEnergy(float fundamental);
        float FitExponentialDecay() const;

        float CalculateSpectralCentroid() const;
        float CalculateInharmonicity(float fundamental);
        std::vector<float> FindHarmonicPeaks(float fundamental) const;

        float NormalizeDecayRate(float decayRate) const;
        float NormalizeSpectralFeatures(float centroid) const;
        void CalculateHealthScore();
        void UpdateResult();

        AnalysisConfig config;

        std::unique_ptr<GuitarDSP::YinPitchDetector> pitchDetector;
        std::unique_ptr<GuitarDSP::FFTProcessor> fftProcessor;

        std::vector<std::vector<float>> harmonicEnergies;
        std::vector<std::chrono::steady_clock::time_point> timestamps;

        float currentFundamental;
        size_t analysisFrameCount;

        float currentHealthScore;
        float currentDecayRate;
        float currentSpectralCentroid;
        float currentInharmonicity;

        mutable std::mutex resultMutex;
        std::shared_ptr<StringHealthResult> latestResult;

        static constexpr size_t g_kFFTSize = 2048;
        static constexpr size_t g_kNumHarmonics = 10;
        static constexpr size_t g_kDecayHistorySize = 50;
        static constexpr float g_kMinDecayRate = -50.0f;
        static constexpr float g_kMaxDecayRate = -5.0f;
    };

} // namespace GuitarDiagnostics::Analysis
