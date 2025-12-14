#include "Analysis/Fretbuzz/FretBuzzDetector.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace GuitarDiagnostics::Analysis
{

    FretBuzzResult::FretBuzzResult()
        : AnalysisResult(), buzzScore(0.0f), onsetDetected(false), transientScore(0.0f), highFreqEnergyScore(0.0f),
          inharmonicityScore(0.0f), stringInfo()
    {
    }

    FretBuzzDetector::FretBuzzDetector()
        : config(0.0f, 0), pitchDetector(nullptr), fftProcessor(nullptr), audioBuffer(),
          prevSpectrum(g_kFFTSize / 2, 0.0f), rmsHistory(10, 0.0f), prevRMS(0.0f), onsetActive(false),
          currentBuzzScore(0.0f), currentOnsetDetected(false), currentTransientScore(0.0f),
          currentHighFreqEnergyScore(0.0f), currentInharmonicityScore(0.0f), currentStringInfo(),
          latestResult(std::make_shared<FretBuzzResult>())
    {
    }

    FretBuzzDetector::~FretBuzzDetector()
    {
    }

    void FretBuzzDetector::Configure(const AnalysisConfig &newConfig)
    {
        config = newConfig;

        GuitarDSP::YinPitchDetectorConfig yinConfig;
        yinConfig.threshold = 0.15f;
        yinConfig.minFrequency = 80.0f;
        yinConfig.maxFrequency = 1200.0f;

        pitchDetector = std::make_unique<GuitarDSP::YinPitchDetector>(yinConfig);
        fftProcessor = std::make_unique<GuitarDSP::FFTProcessor>(g_kFFTSize, config.sampleRate);
    }

    void FretBuzzDetector::ProcessBuffer(std::span<const float> audioData)
    {
        if (!fftProcessor || !pitchDetector)
        {
            return;
        }

        audioBuffer.assign(audioData.begin(), audioData.end());

        fftProcessor->ComputeSpectrum(audioData);

        const auto &spectrum = fftProcessor->GetSpectrum();
        for (size_t i = 0; i < prevSpectrum.size(); ++i)
        {
            prevSpectrum[i] = spectrum.GetMagnitudeAtBin(i);
        }

        bool onset = DetectOnset(audioData);
        currentOnsetDetected = onset;

        currentTransientScore = AnalyzeTransient(audioData);
        currentHighFreqEnergyScore = AnalyzeHighFrequencyNoise();
        currentInharmonicityScore = AnalyzeInharmonicity();

        currentBuzzScore =
            0.3f * currentTransientScore + 0.4f * currentHighFreqEnergyScore + 0.3f * currentInharmonicityScore;

        UpdateResult();
    }

    std::shared_ptr<AnalysisResult> FretBuzzDetector::GetLatestResult() const
    {
        std::lock_guard<std::mutex> lock(resultMutex);
        return latestResult;
    }

    void FretBuzzDetector::Reset()
    {
        prevRMS = 0.0f;
        onsetActive = false;
        std::fill(prevSpectrum.begin(), prevSpectrum.end(), 0.0f);
        std::fill(rmsHistory.begin(), rmsHistory.end(), 0.0f);

        currentBuzzScore = 0.0f;
        currentOnsetDetected = false;
        currentTransientScore = 0.0f;
        currentHighFreqEnergyScore = 0.0f;
        currentInharmonicityScore = 0.0f;

        UpdateResult();
    }

    bool FretBuzzDetector::DetectOnset(std::span<const float> audioData)
    {
        float rms = CalculateRMSEnergy(audioData);
        float spectralFlux = CalculateSpectralFlux();

        bool onset = false;
        if (prevRMS > 0.0f)
        {
            float rmsRatio = rms / prevRMS;
            onset = (rmsRatio > g_kOnsetThreshold) || (spectralFlux > g_kOnsetThreshold);
        }

        prevRMS = rms;
        return onset;
    }

    float FretBuzzDetector::CalculateRMSEnergy(std::span<const float> audioData) const
    {
        float sumSquares = 0.0f;
        for (const auto &sample : audioData)
        {
            sumSquares += sample * sample;
        }

        return std::sqrt(sumSquares / static_cast<float>(audioData.size()));
    }

    float FretBuzzDetector::CalculateSpectralFlux() const
    {
        float flux = 0.0f;
        const auto &spectrum = fftProcessor->GetSpectrum();
        const size_t numBins = std::min(prevSpectrum.size(), static_cast<size_t>(g_kFFTSize / 2));

        for (size_t i = 0; i < numBins; ++i)
        {
            float magnitude = spectrum.GetMagnitudeAtBin(i);
            float diff = magnitude - prevSpectrum[i];
            if (diff > 0.0f)
            {
                flux += diff;
            }
        }

        return flux;
    }

    float FretBuzzDetector::AnalyzeTransient(std::span<const float> audioData)
    {
        float attackTime = CalculateAttackTime(audioData);
        float zcr = CalculateZeroCrossingRate(audioData);

        float attackScore = std::clamp(1.0f - (attackTime / 0.1f), 0.0f, 1.0f);
        float zcrScore = std::clamp(zcr / 1000.0f, 0.0f, 1.0f);

        return (attackScore + zcrScore) / 2.0f;
    }

    float FretBuzzDetector::CalculateAttackTime(std::span<const float> audioData) const
    {
        float maxAmplitude = 0.0f;
        for (const auto &sample : audioData)
        {
            maxAmplitude = std::max(maxAmplitude, std::abs(sample));
        }

        if (maxAmplitude < 0.01f)
        {
            return 1.0f;
        }

        float threshold = maxAmplitude * 0.9f;
        size_t attackSamples = 0;

        for (size_t i = 0; i < audioData.size(); ++i)
        {
            if (std::abs(audioData[i]) >= threshold)
            {
                attackSamples = i;
                break;
            }
        }

        return static_cast<float>(attackSamples) / config.sampleRate;
    }

    float FretBuzzDetector::CalculateZeroCrossingRate(std::span<const float> audioData) const
    {
        if (audioData.size() < 2)
        {
            return 0.0f;
        }

        size_t crossings = 0;
        for (size_t i = 1; i < audioData.size(); ++i)
        {
            if ((audioData[i - 1] >= 0.0f && audioData[i] < 0.0f) || (audioData[i - 1] < 0.0f && audioData[i] >= 0.0f))
            {
                crossings++;
            }
        }

        float duration = static_cast<float>(audioData.size()) / config.sampleRate;
        return static_cast<float>(crossings) / duration;
    }

    float FretBuzzDetector::AnalyzeHighFrequencyNoise()
    {
        const auto &spectrum = fftProcessor->GetSpectrum();
        float highFreqEnergy = spectrum.ExtractBandEnergy(g_kHighFreqMin, g_kHighFreqMax);
        float totalEnergy = spectrum.ExtractBandEnergy(80.0f, 12000.0f);

        if (totalEnergy < 1e-6f)
        {
            return 0.0f;
        }

        return std::clamp(highFreqEnergy / totalEnergy, 0.0f, 1.0f);
    }

    float FretBuzzDetector::AnalyzeInharmonicity()
    {
        if (!pitchDetector || audioBuffer.empty())
        {
            return 0.0f;
        }

        auto pitchResult = pitchDetector->Detect(audioBuffer, config.sampleRate);

        if (!pitchResult.has_value() || pitchResult->confidence < 0.5f)
        {
            return 0.0f;
        }

        float fundamental = pitchResult->frequency;

        if (pitchResult->confidence > 0.85f)
        {
            currentStringInfo = StringDetector::Classify(fundamental);
        }

        auto harmonics = ExtractHarmonics(fundamental);

        return CalculateInharmonicityMetric(harmonics, fundamental);
    }

    std::vector<float> FretBuzzDetector::ExtractHarmonics(float fundamental) const
    {
        std::vector<float> harmonics;
        harmonics.reserve(g_kNumHarmonics);

        if (config.sampleRate <= 0.0f)
        {
            return harmonics;
        }

        const auto &spectrum = fftProcessor->GetSpectrum();

        for (size_t n = 1; n <= g_kNumHarmonics; ++n)
        {
            float expectedFreq = fundamental * static_cast<float>(n);
            float magnitude = spectrum.GetMagnitudeAtFrequency(expectedFreq);
            harmonics.push_back(magnitude);
        }

        return harmonics;
    }

    float FretBuzzDetector::CalculateInharmonicityMetric(const std::vector<float> &harmonics, float fundamental) const
    {
        if (harmonics.empty() || fundamental <= 0.0f)
        {
            return 0.0f;
        }

        const auto &spectrum = fftProcessor->GetSpectrum();
        float totalDeviation = 0.0f;
        float binWidth = config.sampleRate / static_cast<float>(g_kFFTSize);

        for (size_t n = 0; n < harmonics.size(); ++n)
        {
            float expectedFreq = fundamental * static_cast<float>(n + 1);
            size_t expectedBin = static_cast<size_t>(expectedFreq / binWidth);

            float maxMag = 0.0f;
            size_t actualBin = expectedBin;

            for (int offset = -2; offset <= 2; ++offset)
            {
                int checkBin = static_cast<int>(expectedBin) + offset;
                if (checkBin >= 0 && checkBin < static_cast<int>(g_kFFTSize / 2))
                {
                    float mag = spectrum.GetMagnitudeAtBin(static_cast<size_t>(checkBin));
                    if (mag > maxMag)
                    {
                        maxMag = mag;
                        actualBin = static_cast<size_t>(checkBin);
                    }
                }
            }

            float actualFreq = static_cast<float>(actualBin) * binWidth;
            float deviation = std::abs(actualFreq - expectedFreq) / expectedFreq;
            totalDeviation += deviation;
        }

        return std::clamp(totalDeviation / static_cast<float>(harmonics.size()), 0.0f, 1.0f);
    }

    void FretBuzzDetector::UpdateResult()
    {
        auto result = std::make_shared<FretBuzzResult>();
        result->timestamp = std::chrono::system_clock::now();
        result->isValid = true;
        result->buzzScore = currentBuzzScore;
        result->onsetDetected = currentOnsetDetected;
        result->transientScore = currentTransientScore;
        result->highFreqEnergyScore = currentHighFreqEnergyScore;
        result->inharmonicityScore = currentInharmonicityScore;
        result->stringInfo = currentStringInfo;

        std::lock_guard<std::mutex> lock(resultMutex);
        latestResult = std::move(result);
    }

} // namespace GuitarDiagnostics::Analysis
