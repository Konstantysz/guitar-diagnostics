#pragma once

#include "Analysis/Analyzer.h"

#include <YinPitchDetector.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    /**
     * @brief States for the intonation analysis workflow.
     */
    enum class IntonationState
    {
        Idle,            ///< Waiting for input.
        OpenString,      ///< Analyzing open string pitch.
        WaitFor12thFret, ///< Prompting user to play 12th fret.
        FrettedString,   ///< Analyzing 12th fret pitch.
        Complete         ///< Analysis complete.
    };

    /**
     * @brief Result structure for intonation analysis.
     */
    struct IntonationResult : public AnalysisResult
    {
        IntonationState state;          ///< Current analysis state.
        float openStringFrequency;      ///< Detect frequency of the open string.
        float frettedStringFrequency;   ///< Detect frequency of the fretted string.
        float expectedFrettedFrequency; ///< Expected frequency for the fretted string.
        float centDeviation;            ///< Deviation in cents.
        bool isInTune;                  ///< True if intonation is within tolerance.

        /**
         * @brief Constructs an IntonationResult with default values.
         */
        IntonationResult();
    };

    /**
     * @brief Analyzer for checking guitar intonation.
     *
     * Guides the user through comparing open string pitch vs 12th fret pitch.
     */
    class IntonationAnalyzer : public Analyzer
    {
    public:
        /**
         * @brief Constructs the IntonationAnalyzer.
         */
        IntonationAnalyzer();

        /**
         * @brief Destructor.
         */
        ~IntonationAnalyzer() override;

        IntonationAnalyzer(const IntonationAnalyzer &) = delete;

        IntonationAnalyzer &operator=(const IntonationAnalyzer &) = delete;

        IntonationAnalyzer(IntonationAnalyzer &&) = delete;

        IntonationAnalyzer &operator=(IntonationAnalyzer &&) = delete;

        void Configure(const AnalysisConfig &config) override;

        void ProcessBuffer(std::span<const float> audioData) override;

        std::shared_ptr<AnalysisResult> GetLatestResult() const override;

        void Reset() override;

    private:
        /**
         * @brief Updates the analysis state machine based on pitch input.
         * @param frequency Detected pitch frequency in Hz.
         * @param confidence Confidence level of the pitch detection (0.0 to 1.0).
         */
        void UpdateStateMachine(float frequency, float confidence);

        /**
         * @brief Transitions to the OpenString state.
         * @param frequency The detected open string frequency.
         */
        void TransitionToOpenString(float frequency);

        /** @brief Transitions to the WaitFor12thFret state. */
        void TransitionToWaitFor12thFret();

        /**
         * @brief Transitions to the FrettedString state.
         * @param frequency The detected fretted string frequency.
         */
        void TransitionToFrettedString(float frequency);

        /** @brief Transitions to the Complete state. */
        void TransitionToComplete();

        /**
         * @brief Accumulates pitch samples for stability analysis.
         * @param frequency The pitch sample to accumulate.
         */
        void AccumulatePitch(float frequency);

        /**
         * @brief Calculates the stable pitch from accumulated samples.
         * @return The average stable pitch frequency.
         */
        float GetStablePitch() const;

        /**
         * @brief Checks if the accumulated pitch is stable.
         * @return True if pitch is stable, false otherwise.
         */
        bool HasStablePitch() const;

        /**
         * @brief Calculates standard deviation of accumulated pitch.
         * @return Standard deviation value.
         */
        float CalculateStandardDeviation() const;

        /** @brief Calculates the intonation deviation. */
        void CalculateDeviation();

        /** @brief Updates the shared result structure. */
        void UpdateResult();

        AnalysisConfig config; ///< Analysis configuration.

        std::unique_ptr<GuitarDSP::YinPitchDetector> pitchDetector; ///< Pitch detector instance.

        IntonationState currentState;                         ///< Current state of the intonation check.
        std::vector<float> pitchAccumulator;                  ///< Buffer for accumulating pitch samples.
        size_t pitchCount;                                    ///< Number of accumulated pitch samples.
        std::chrono::steady_clock::time_point stateStartTime; ///< Time when the current state started.

        float openStringFreq;    ///< Measured open string frequency.
        float frettedStringFreq; ///< Measured fretted string frequency.
        float centDeviation;     ///< Calculated deviation in cents.
        bool isInTune;           ///< Intonation check result.

        mutable std::mutex resultMutex;                 ///< Mutex for thread-safe result access.
        std::shared_ptr<IntonationResult> latestResult; ///< The latest analysis result.

        static constexpr float g_kConfidenceThreshold = 0.7f;  ///< Pitch detection confidence threshold.
        static constexpr size_t g_kPitchAccumulatorSize = 100; ///< Number of samples to accumulate for stability.
        static constexpr std::chrono::milliseconds g_kStableTimeRequired{
            500
        }; ///< Time required for pitch to be considered stable.
        static constexpr float g_kInTuneTolerance = 5.0f;    ///< Tolerance in cents for being "in tune".
        static constexpr float g_kStabilityThreshold = 2.0f; ///< Standard deviation threshold for pitch stability.
    };

} // namespace GuitarDiagnostics::Analysis
