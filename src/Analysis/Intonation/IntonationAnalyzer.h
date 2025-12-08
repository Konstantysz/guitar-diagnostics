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

    enum class IntonationState
    {
        Idle,
        OpenString,
        WaitFor12thFret,
        FrettedString,
        Complete
    };

    struct IntonationResult : public AnalysisResult
    {
        IntonationState state;
        float openStringFrequency;
        float frettedStringFrequency;
        float expectedFrettedFrequency;
        float centDeviation;
        bool isInTune;

        IntonationResult();
    };

    class IntonationAnalyzer : public Analyzer
    {
    public:
        IntonationAnalyzer();
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
        void UpdateStateMachine(float frequency, float confidence);
        void TransitionToOpenString(float frequency);
        void TransitionToWaitFor12thFret();
        void TransitionToFrettedString(float frequency);
        void TransitionToComplete();

        void AccumulatePitch(float frequency);
        float GetStablePitch() const;
        bool HasStablePitch() const;
        float CalculateStandardDeviation() const;

        void CalculateDeviation();
        void UpdateResult();

        AnalysisConfig config;

        std::unique_ptr<GuitarDSP::YinPitchDetector> pitchDetector;

        IntonationState currentState;
        std::vector<float> pitchAccumulator;
        size_t pitchCount;
        std::chrono::steady_clock::time_point stateStartTime;

        float openStringFreq;
        float frettedStringFreq;
        float centDeviation;
        bool isInTune;

        mutable std::mutex resultMutex;
        std::shared_ptr<IntonationResult> latestResult;

        static constexpr float g_kConfidenceThreshold = 0.7f;
        static constexpr size_t g_kPitchAccumulatorSize = 100;
        static constexpr std::chrono::milliseconds g_kStableTimeRequired{ 500 };
        static constexpr float g_kInTuneTolerance = 5.0f;
        static constexpr float g_kStabilityThreshold = 2.0f;
    };

} // namespace GuitarDiagnostics::Analysis
