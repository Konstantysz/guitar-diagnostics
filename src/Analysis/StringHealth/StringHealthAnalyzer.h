#pragma once

#include "Analysis/Analyzer.h"
#include "Analysis/StringDetector.h"

#include <FFTProcessor.h>
#include <YinPitchDetector.h>

#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    /**
     * @brief Result structure for string health analysis.
     */
    struct StringHealthResult : public AnalysisResult
    {
        float healthScore;          ///< Overall health score (0.0 to 100.0).
        float decayRate;            ///< Rate of signal decay.
        float spectralCentroid;     ///< Spectral centroid position.
        float inharmonicity;        ///< Inharmonicity measure.
        float fundamentalFrequency; ///< Fundamental frequency of the string.
        StringInfo stringInfo;      ///< Detected string information.

        /**
         * @brief Constructs a StringHealthResult with default values.
         */
        StringHealthResult();
    };

    /**
     * @brief Analyzer for assessing the physical condition of strings.
     *
     * Evaluates brightness, sustain, and inharmonicity to determine string age and quality.
     */
    class StringHealthAnalyzer : public Analyzer
    {
    public:
        /**
         * @brief Constructs the StringHealthAnalyzer.
         */
        StringHealthAnalyzer();

        /**
         * @brief Destructor.
         */
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
        /**
         * @brief Analyzes the amplitude decay envelope.
         * @return The calculated decay rate.
         */
        float AnalyzeDecay();

        /**
         * @brief Tracks energy in harmonic bands over time.
         * @param fundamental The fundamental frequency to base harmonic bands on.
         */
        void TrackHarmonicEnergy(float fundamental);

        /**
         * @brief Fits an exponential decay curve to the energy history.
         * @return The decay coefficient.
         */
        float FitExponentialDecay() const;

        /**
         * @brief Calculates the center of mass of the spectrum.
         * @return Spectral centroid value.
         */
        float CalculateSpectralCentroid() const;

        /**
         * @brief Calculates inharmonicity based on harmonic positions.
         * @param fundamental The expected fundamental frequency.
         * @return Inharmonicity score.
         */
        float CalculateInharmonicity(float fundamental);

        /**
         * @brief Identifies harmonic peaks given a fundamental.
         * @param fundamental The fundamental frequency.
         * @return A vector of harmonic peak frequencies.
         */
        std::vector<float> FindHarmonicPeaks(float fundamental) const;

        /**
         * @brief Normalizes the raw decay rate to a 0-1 scale.
         * @param decayRate The raw decay rate.
         * @return Normalized health sub-score for decay.
         */
        float NormalizeDecayRate(float decayRate) const;

        /**
         * @brief Normalizes spectral centroid and other features.
         * @param centroid The raw spectral centroid.
         * @return Normalized health sub-score for spectral features.
         */
        float NormalizeSpectralFeatures(float centroid) const;

        /** @brief Combines metrics into an overall health score. */
        void CalculateHealthScore();

        /** @brief Updates the shared result structure. */
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
