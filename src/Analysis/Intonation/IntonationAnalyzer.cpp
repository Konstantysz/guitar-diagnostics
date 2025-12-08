#include "Analysis/Intonation/IntonationAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace GuitarDiagnostics::Analysis
{

    IntonationResult::IntonationResult()
        : AnalysisResult(), state(IntonationState::Idle), openStringFrequency(0.0f), frettedStringFrequency(0.0f),
          expectedFrettedFrequency(0.0f), centDeviation(0.0f), isInTune(false)
    {
    }

    IntonationAnalyzer::IntonationAnalyzer()
        : config(0.0f, 0), pitchDetector(nullptr), currentState(IntonationState::Idle),
          pitchAccumulator(g_kPitchAccumulatorSize, 0.0f), pitchCount(0),
          stateStartTime(std::chrono::steady_clock::now()), openStringFreq(0.0f), frettedStringFreq(0.0f),
          centDeviation(0.0f), isInTune(false), latestResult(std::make_shared<IntonationResult>())
    {
    }

    IntonationAnalyzer::~IntonationAnalyzer()
    {
    }

    void IntonationAnalyzer::Configure(const AnalysisConfig &newConfig)
    {
        config = newConfig;

        GuitarDSP::YinPitchDetectorConfig yinConfig;
        yinConfig.threshold = 0.15f;
        yinConfig.minFrequency = 80.0f;
        yinConfig.maxFrequency = 1200.0f;

        pitchDetector = std::make_unique<GuitarDSP::YinPitchDetector>(yinConfig);
    }

    void IntonationAnalyzer::ProcessBuffer(std::span<const float> audioData)
    {
        if (!pitchDetector)
        {
            return;
        }

        auto pitchResult = pitchDetector->Detect(audioData, config.sampleRate);

        if (pitchResult.has_value())
        {
            float frequency = pitchResult->frequency;
            float confidence = pitchResult->confidence;

            if (confidence >= g_kConfidenceThreshold)
            {
                AccumulatePitch(frequency);
                UpdateStateMachine(frequency, confidence);
            }
        }

        UpdateResult();
    }

    std::shared_ptr<AnalysisResult> IntonationAnalyzer::GetLatestResult() const
    {
        std::lock_guard<std::mutex> lock(resultMutex);
        return latestResult;
    }

    void IntonationAnalyzer::Reset()
    {
        currentState = IntonationState::Idle;
        pitchCount = 0;
        std::fill(pitchAccumulator.begin(), pitchAccumulator.end(), 0.0f);
        openStringFreq = 0.0f;
        frettedStringFreq = 0.0f;
        centDeviation = 0.0f;
        isInTune = false;
        stateStartTime = std::chrono::steady_clock::now();

        UpdateResult();
    }

    void IntonationAnalyzer::UpdateStateMachine([[maybe_unused]] float frequency, [[maybe_unused]] float confidence)
    {
        switch (currentState)
        {
        case IntonationState::Idle:
            if (HasStablePitch())
            {
                TransitionToOpenString(GetStablePitch());
            }
            break;

        case IntonationState::OpenString:
            if (HasStablePitch())
            {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateStartTime);

                if (elapsed >= g_kStableTimeRequired)
                {
                    TransitionToWaitFor12thFret();
                }
            }
            break;

        case IntonationState::WaitFor12thFret:
            if (HasStablePitch())
            {
                float currentPitch = GetStablePitch();
                float expectedFretted = openStringFreq * 2.0f;

                if (std::abs(currentPitch - expectedFretted) / expectedFretted < 0.1f)
                {
                    TransitionToFrettedString(currentPitch);
                }
            }
            break;

        case IntonationState::FrettedString:
            if (HasStablePitch())
            {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - stateStartTime);

                if (elapsed >= g_kStableTimeRequired)
                {
                    TransitionToComplete();
                }
            }
            break;

        case IntonationState::Complete:
            break;
        }
    }

    void IntonationAnalyzer::TransitionToOpenString(float frequency)
    {
        currentState = IntonationState::OpenString;
        openStringFreq = frequency;
        pitchCount = 0;
        stateStartTime = std::chrono::steady_clock::now();
    }

    void IntonationAnalyzer::TransitionToWaitFor12thFret()
    {
        currentState = IntonationState::WaitFor12thFret;
        pitchCount = 0;
        stateStartTime = std::chrono::steady_clock::now();
    }

    void IntonationAnalyzer::TransitionToFrettedString(float frequency)
    {
        currentState = IntonationState::FrettedString;
        frettedStringFreq = frequency;
        pitchCount = 0;
        stateStartTime = std::chrono::steady_clock::now();
    }

    void IntonationAnalyzer::TransitionToComplete()
    {
        currentState = IntonationState::Complete;
        CalculateDeviation();
    }

    void IntonationAnalyzer::AccumulatePitch(float frequency)
    {
        if (pitchCount < g_kPitchAccumulatorSize)
        {
            pitchAccumulator[pitchCount] = frequency;
            pitchCount++;
        }
        else
        {
            std::shift_left(pitchAccumulator.begin(), pitchAccumulator.end(), 1);
            pitchAccumulator[g_kPitchAccumulatorSize - 1] = frequency;
        }
    }

    float IntonationAnalyzer::GetStablePitch() const
    {
        if (pitchCount == 0)
        {
            return 0.0f;
        }

        std::vector<float> sortedPitches(pitchAccumulator.begin(), pitchAccumulator.begin() + pitchCount);
        std::sort(sortedPitches.begin(), sortedPitches.end());

        size_t medianIndex = sortedPitches.size() / 2;
        if (sortedPitches.size() % 2 == 0)
        {
            return (sortedPitches[medianIndex - 1] + sortedPitches[medianIndex]) / 2.0f;
        }
        else
        {
            return sortedPitches[medianIndex];
        }
    }

    bool IntonationAnalyzer::HasStablePitch() const
    {
        if (pitchCount < 10)
        {
            return false;
        }

        float stdDev = CalculateStandardDeviation();
        return stdDev < g_kStabilityThreshold;
    }

    float IntonationAnalyzer::CalculateStandardDeviation() const
    {
        if (pitchCount == 0)
        {
            return 0.0f;
        }

        float mean = std::accumulate(pitchAccumulator.begin(), pitchAccumulator.begin() + pitchCount, 0.0f)
                     / static_cast<float>(pitchCount);

        float variance = 0.0f;
        for (size_t i = 0; i < pitchCount; ++i)
        {
            float diff = pitchAccumulator[i] - mean;
            variance += diff * diff;
        }
        variance /= static_cast<float>(pitchCount);

        return std::sqrt(variance);
    }

    void IntonationAnalyzer::CalculateDeviation()
    {
        float expectedFretted = openStringFreq * 2.0f;

        if (frettedStringFreq > 0.0f && expectedFretted > 0.0f)
        {
            centDeviation = 1200.0f * std::log2(frettedStringFreq / expectedFretted);
            isInTune = std::abs(centDeviation) <= g_kInTuneTolerance;
        }
        else
        {
            centDeviation = 0.0f;
            isInTune = false;
        }
    }

    void IntonationAnalyzer::UpdateResult()
    {
        auto result = std::make_shared<IntonationResult>();
        result->timestamp = std::chrono::system_clock::now();
        result->isValid = true;
        result->state = currentState;
        result->openStringFrequency = openStringFreq;
        result->frettedStringFrequency = frettedStringFreq;
        result->expectedFrettedFrequency = openStringFreq * 2.0f;
        result->centDeviation = centDeviation;
        result->isInTune = isInTune;

        std::lock_guard<std::mutex> lock(resultMutex);
        latestResult = std::move(result);
    }

} // namespace GuitarDiagnostics::Analysis
