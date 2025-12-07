#pragma once

#include <chrono>
#include <memory>
#include <span>
#include <string>

namespace GuitarDiagnostics::Analysis
{

    struct AnalysisConfig
    {
        float sampleRate;
        uint32_t bufferSize;

        AnalysisConfig(float sampleRate, uint32_t bufferSize);
    };

    struct AnalysisResult
    {
        std::chrono::system_clock::time_point timestamp;
        bool isValid;
        std::string errorMessage;

        AnalysisResult();
        virtual ~AnalysisResult() = default;
    };

    class Analyzer
    {
    public:
        virtual ~Analyzer() = default;

        virtual void Configure(const AnalysisConfig &config) = 0;
        virtual void ProcessBuffer(std::span<const float> audioData) = 0;
        virtual std::shared_ptr<AnalysisResult> GetLatestResult() const = 0;
        virtual void Reset() = 0;

    protected:
        Analyzer() = default;
        Analyzer(const Analyzer &) = default;
        Analyzer(Analyzer &&) noexcept = default;
        Analyzer &operator=(const Analyzer &) = default;
        Analyzer &operator=(Analyzer &&) noexcept = default;
    };

} // namespace GuitarDiagnostics::Analysis
