#pragma once

#include "Analysis/Analyzer.h"
#include "Analysis/StringDetector.h"

#include <FFTProcessor.h>
#include <YinPitchDetector.h>

#include <memory>
#include <mutex>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    /**
     * @brief Result structure for fret buzz analysis.
     */
    struct FretBuzzResult : public AnalysisResult
    {
        float buzzScore;           ///< Calculated buzz score (0.0 to 1.0).
        bool onsetDetected;        ///< Flag indicating if a note onset was detected.
        float transientScore;      ///< Transient analysis score.
        float highFreqEnergyScore; ///< High frequency energy metric.
        float inharmonicityScore;  ///< Inharmonicity metric.
        StringInfo stringInfo;     ///< Detected string information.

        /**
         * @brief Constructs a FretBuzzResult with default values.
         */
        FretBuzzResult();
    };

    /**
     * @brief Analyzer for detecting fret buzz and mechanical noise.
     *
     * Uses spectral analysis to identify high-frequency noise and inharmonicity
     * characteristic of fret buzz.
     */
    class FretBuzzDetector : public Analyzer
    {
    public:
        /**
         * @brief Constructs the FretBuzzDetector.
         */
        FretBuzzDetector();

        /**
         * @brief Destructor.
         */
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
        /**
         * @brief Detects note onsets in the audio signal.
         * @param audioData Input audio buffer.
         * @return True if onset detected, false otherwise.
         */
        bool DetectOnset(std::span<const float> audioData);

        /**
         * @brief Calculates Root Mean Square energy.
         * @param audioData Input audio buffer.
         * @return RMS value.
         */
        float CalculateRMSEnergy(std::span<const float> audioData) const;

        /**
         * @brief Calculates change in spectral energy.
         * @return Spectral flux value.
         */
        float CalculateSpectralFlux() const;

        /**
         * @brief Analyzes transient characteristics.
         * @param audioData Input audio buffer.
         * @return Transient score.
         */
        float AnalyzeTransient(std::span<const float> audioData);

        /**
         * @brief Calculates signal attack time.
         * @param audioData Input audio buffer.
         * @return Attack time in milliseconds.
         */
        float CalculateAttackTime(std::span<const float> audioData) const;

        /**
         * @brief Calculates zero crossing rate.
         * @param audioData Input audio buffer.
         * @return Zero crossing rate.
         */
        float CalculateZeroCrossingRate(std::span<const float> audioData) const;

        /**
         * @brief Analyzes noise energy in high frequency bands.
         * @return High frequency energy score.
         */
        float AnalyzeHighFrequencyNoise();

        /**
         * @brief Analyzes signal inharmonicity.
         * @return Inharmonicity score.
         */
        float AnalyzeInharmonicity();

        /**
         * @brief Extracts harmonic peaks from the spectrum.
         * @param fundamental The fundamental frequency.
         * @return Vector of harmonic peak frequencies.
         */
        std::vector<float> ExtractHarmonics(float fundamental) const;

        /**
         * @brief Calculates inharmonicity score from harmonics.
         * @param harmonics Vector of harmonic frequencies.
         * @param fundamental The fundamental frequency.
         * @return Inharmonicity metric.
         */
        float CalculateInharmonicityMetric(const std::vector<float> &harmonics, float fundamental) const;

        /**
         * @brief Updates the shared result structure.
         */
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
        StringInfo currentStringInfo;

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
