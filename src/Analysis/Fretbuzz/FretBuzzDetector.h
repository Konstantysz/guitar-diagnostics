#pragma once

#include "Analysis/Analyzer.h"

#include <FFTProcessor.h>
#include <YinPitchDetector.h>

#include <memory>
#include <mutex>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    struct FretBuzzResult : public AnalysisResult
    {
        float buzzScore;
        bool onsetDetected;
        float transientScore;
        float highFreqEnergyScore;
        float inharmonicityScore;

        FretBuzzResult();
    };

    class FretBuzzDetector : public Analyzer
    {
    public:
        FretBuzzDetector();
        ~FretBuzzDetector() override;

        FretBuzzDetector(const FretBuzzDetector &) = delete;
        FretBuzzDetector &operator=(const FretBuzzDetector &) = delete;
        FretBuzzDetector(FretBuzzDetector &&) = delete;
        FretBuzzDetector &operator=(FretBuzzDetector &&) = delete;

        void Configure(const AnalysisConfig &config) override;
        void ProcessBuffer(std::span<const float> audioData) override;
        std::shared_ptr<AnalysisResult> GetLatestResult() const override;
        void Reset() override;

    private:
        bool DetectOnset(std::span<const float> audioData);
        float CalculateRMSEnergy(std::span<const float> audioData) const;
        float CalculateSpectralFlux() const;

        float AnalyzeTransient(std::span<const float> audioData);
        float CalculateAttackTime(std::span<const float> audioData) const;
        float CalculateZeroCrossingRate(std::span<const float> audioData) const;

        float AnalyzeHighFrequencyNoise();
        float AnalyzeInharmonicity();
        std::vector<float> ExtractHarmonics(float fundamental) const;
        float CalculateInharmonicityMetric(const std::vector<float> &harmonics, float fundamental) const;

        void UpdateResult();

        AnalysisConfig config;

        std::unique_ptr<GuitarDSP::YinPitchDetector> pitchDetector;
        std::unique_ptr<GuitarDSP::FFTProcessor> fftProcessor;

        std::vector<float> audioBuffer;
        std::vector<float> prevSpectrum;
        std::vector<float> rmsHistory;

        float prevRMS;
        bool onsetActive;

        float currentBuzzScore;
        bool currentOnsetDetected;
        float currentTransientScore;
        float currentHighFreqEnergyScore;
        float currentInharmonicityScore;

        mutable std::mutex resultMutex;
        std::shared_ptr<FretBuzzResult> latestResult;

        static constexpr size_t g_kFFTSize = 2048;
        static constexpr float g_kOnsetThreshold = 1.5f;
        static constexpr float g_kBuzzThreshold = 0.3f;
        static constexpr float g_kHighFreqMin = 4000.0f;
        static constexpr float g_kHighFreqMax = 8000.0f;
        static constexpr size_t g_kNumHarmonics = 10;
    };

} // namespace GuitarDiagnostics::Analysis
