#include "Analysis/StringHealth/StringHealthAnalyzer.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace GuitarDiagnostics::Analysis
{

    StringHealthResult::StringHealthResult()
        : AnalysisResult(), healthScore(0.0f), decayRate(0.0f), spectralCentroid(0.0f), inharmonicity(0.0f),
          fundamentalFrequency(0.0f), stringInfo()
    {
    }

    StringHealthAnalyzer::StringHealthAnalyzer()
        : config(0.0f, 0), pitchDetector(nullptr), fftProcessor(nullptr), harmonicEnergies(), timestamps(),
          currentFundamental(0.0f), analysisFrameCount(0), currentHealthScore(0.0f), currentDecayRate(0.0f),
          currentSpectralCentroid(0.0f), currentInharmonicity(0.0f), currentStringInfo(),
          latestResult(std::make_shared<StringHealthResult>())
    {
        harmonicEnergies.reserve(g_kDecayHistorySize);
        timestamps.reserve(g_kDecayHistorySize);
    }

    StringHealthAnalyzer::~StringHealthAnalyzer()
    {
    }

    void StringHealthAnalyzer::Configure(const AnalysisConfig &newConfig)
    {
        config = newConfig;

        GuitarDSP::YinPitchDetectorConfig yinConfig;
        yinConfig.threshold = 0.15f;
        yinConfig.minFrequency = 80.0f;
        yinConfig.maxFrequency = 1200.0f;

        pitchDetector = std::make_unique<GuitarDSP::YinPitchDetector>(yinConfig);
        fftProcessor = std::make_unique<GuitarDSP::FFTProcessor>(g_kFFTSize, config.sampleRate);
    }

    void StringHealthAnalyzer::ProcessBuffer(std::span<const float> audioData)
    {
        if (!fftProcessor || !pitchDetector)
        {
            return;
        }

        fftProcessor->ComputeSpectrum(audioData);

        auto pitchResult = pitchDetector->Detect(audioData, config.sampleRate);

        if (pitchResult.has_value() && pitchResult->confidence > 0.5f)
        {
            currentFundamental = pitchResult->frequency;
            TrackHarmonicEnergy(currentFundamental);

            if (pitchResult->confidence > 0.85f && analysisFrameCount > 3)
            {
                currentStringInfo = StringDetector::Classify(currentFundamental);
            }
        }

        currentDecayRate = AnalyzeDecay();
        currentSpectralCentroid = CalculateSpectralCentroid();
        currentInharmonicity = CalculateInharmonicity(currentFundamental);

        CalculateHealthScore();
        UpdateResult();

        analysisFrameCount++;
    }

    std::shared_ptr<AnalysisResult> StringHealthAnalyzer::GetLatestResult() const
    {
        std::lock_guard<std::mutex> lock(resultMutex);
        return latestResult;
    }

    void StringHealthAnalyzer::Reset()
    {
        currentFundamental = 0.0f;
        analysisFrameCount = 0;
        currentHealthScore = 0.0f;
        currentDecayRate = 0.0f;
        currentSpectralCentroid = 0.0f;
        currentInharmonicity = 0.0f;

        harmonicEnergies.clear();
        timestamps.clear();

        UpdateResult();
    }

    float StringHealthAnalyzer::AnalyzeDecay()
    {
        if (harmonicEnergies.size() < 10)
        {
            return 0.0f;
        }

        return FitExponentialDecay();
    }

    void StringHealthAnalyzer::TrackHarmonicEnergy(float fundamental)
    {
        std::vector<float> energies;
        energies.reserve(g_kNumHarmonics);

        for (size_t n = 1; n <= g_kNumHarmonics; ++n)
        {
            float harmonicFreq = fundamental * static_cast<float>(n);
            float magnitude = fftProcessor->GetSpectrum().GetMagnitudeAtFrequency(harmonicFreq);
            energies.push_back(magnitude);
        }

        harmonicEnergies.push_back(energies);
        timestamps.push_back(std::chrono::steady_clock::now());

        if (harmonicEnergies.size() > g_kDecayHistorySize)
        {
            harmonicEnergies.erase(harmonicEnergies.begin());
            timestamps.erase(timestamps.begin());
        }
    }

    float StringHealthAnalyzer::FitExponentialDecay() const
    {
        if (harmonicEnergies.size() < 2 || timestamps.size() < 2)
        {
            return 0.0f;
        }

        std::vector<float> avgEnergies;
        avgEnergies.reserve(harmonicEnergies.size());

        for (const auto &energies : harmonicEnergies)
        {
            float avg = std::accumulate(energies.begin(), energies.end(), 0.0f) / static_cast<float>(energies.size());
            avgEnergies.push_back(avg);
        }

        std::vector<float> logEnergies;
        std::vector<float> times;

        for (size_t i = 0; i < avgEnergies.size(); ++i)
        {
            if (avgEnergies[i] > 1e-6f)
            {
                logEnergies.push_back(std::log(avgEnergies[i]));
                auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(timestamps[i] - timestamps[0]).count();
                times.push_back(static_cast<float>(duration) / 1000.0f);
            }
        }

        if (logEnergies.size() < 2)
        {
            return 0.0f;
        }

        float meanTime = std::accumulate(times.begin(), times.end(), 0.0f) / static_cast<float>(times.size());
        float meanLogE =
            std::accumulate(logEnergies.begin(), logEnergies.end(), 0.0f) / static_cast<float>(logEnergies.size());

        float numerator = 0.0f;
        float denominator = 0.0f;

        for (size_t i = 0; i < times.size(); ++i)
        {
            float tDiff = times[i] - meanTime;
            float eDiff = logEnergies[i] - meanLogE;
            numerator += tDiff * eDiff;
            denominator += tDiff * tDiff;
        }

        if (denominator < 1e-6f)
        {
            return 0.0f;
        }

        float slope = numerator / denominator;
        float decayRateDbPerSec = slope * 8.686f;

        return decayRateDbPerSec;
    }

    float StringHealthAnalyzer::CalculateSpectralCentroid() const
    {
        return fftProcessor->GetSpectrum().CalculateSpectralCentroid();
    }


    float StringHealthAnalyzer::CalculateInharmonicity(float fundamental)
    {
        if (fundamental <= 0.0f)
        {
            return 0.0f;
        }

        auto harmonicPeaks = FindHarmonicPeaks(fundamental);

        if (harmonicPeaks.empty())
        {
            return 0.0f;
        }

        float totalDeviation = 0.0f;
        for (size_t n = 0; n < harmonicPeaks.size(); ++n)
        {
            float expectedFreq = fundamental * static_cast<float>(n + 1);
            float actualFreq = harmonicPeaks[n];

            if (expectedFreq > 0.0f && actualFreq > 0.0f)
            {
                float deviation = std::abs(actualFreq - expectedFreq) / expectedFreq;
                totalDeviation += deviation;
            }
        }

        return std::clamp(totalDeviation / static_cast<float>(harmonicPeaks.size()), 0.0f, 1.0f);
    }

    std::vector<float> StringHealthAnalyzer::FindHarmonicPeaks(float fundamental) const
    {
        std::vector<float> peaks;
        peaks.reserve(g_kNumHarmonics);

        if (config.sampleRate <= 0.0f)
        {
            return peaks;
        }

        const auto &spectrum = fftProcessor->GetSpectrum();
        float binWidth = config.sampleRate / static_cast<float>(g_kFFTSize);

        for (size_t n = 1; n <= g_kNumHarmonics; ++n)
        {
            float expectedFreq = fundamental * static_cast<float>(n);
            size_t expectedBin = static_cast<size_t>(expectedFreq / binWidth);

            float maxMag = 0.0f;
            size_t peakBin = expectedBin;

            for (int offset = -3; offset <= 3; ++offset)
            {
                int checkBin = static_cast<int>(expectedBin) + offset;
                if (checkBin >= 0 && checkBin < static_cast<int>(g_kFFTSize / 2))
                {
                    float mag = spectrum.GetMagnitudeAtBin(static_cast<size_t>(checkBin));

                    if (mag > maxMag)
                    {
                        maxMag = mag;
                        peakBin = static_cast<size_t>(checkBin);
                    }
                }
            }

            float peakFreq = static_cast<float>(peakBin) * binWidth;
            peaks.push_back(peakFreq);
        }

        return peaks;
    }

    float StringHealthAnalyzer::NormalizeDecayRate(float decayRate) const
    {
        float normalized = (decayRate - g_kMinDecayRate) / (g_kMaxDecayRate - g_kMinDecayRate);
        return std::clamp(normalized, 0.0f, 1.0f);
    }

    float StringHealthAnalyzer::NormalizeSpectralFeatures(float centroid) const
    {
        float normalizedCentroid = centroid / 5000.0f;
        return std::clamp(1.0f - normalizedCentroid, 0.0f, 1.0f);
    }

    void StringHealthAnalyzer::CalculateHealthScore()
    {
        float decayScore = NormalizeDecayRate(currentDecayRate);
        float spectralScore = NormalizeSpectralFeatures(currentSpectralCentroid);
        float inharmonicityScore = 1.0f - currentInharmonicity;

        currentHealthScore = 0.3f * decayScore + 0.3f * spectralScore + 0.4f * inharmonicityScore;
        currentHealthScore = std::clamp(currentHealthScore, 0.0f, 1.0f);
    }

    void StringHealthAnalyzer::UpdateResult()
    {
        auto result = std::make_shared<StringHealthResult>();
        result->timestamp = std::chrono::system_clock::now();
        result->isValid = true;
        result->healthScore = currentHealthScore;
        result->decayRate = currentDecayRate;
        result->spectralCentroid = currentSpectralCentroid;
        result->inharmonicity = currentInharmonicity;
        result->fundamentalFrequency = currentFundamental;
        result->stringInfo = currentStringInfo;

        std::lock_guard<std::mutex> lock(resultMutex);
        latestResult = std::move(result);
    }

} // namespace GuitarDiagnostics::Analysis
