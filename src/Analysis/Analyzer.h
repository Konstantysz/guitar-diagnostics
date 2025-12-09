#pragma once

#include <chrono>
#include <memory>
#include <span>
#include <string>

namespace GuitarDiagnostics::Analysis
{

    /**
     * @brief Configuration parameters for audio analysis.
     */
    struct AnalysisConfig
    {
        float sampleRate;    ///< Audio sample rate in Hz.
        uint32_t bufferSize; ///< Size of the audio buffer in frames.

        /**
         * @brief Constructs an AnalysisConfig.
         * @param sampleRate The sample rate in Hz.
         * @param bufferSize The buffer size in frames.
         */
        AnalysisConfig(float sampleRate, uint32_t bufferSize);
    };

    /**
     * @brief Base struct for analysis results.
     */
    struct AnalysisResult
    {
        std::chrono::system_clock::time_point timestamp; ///< Time when the result was generated.
        bool isValid;                                    ///< Validity flag for the result.
        std::string errorMessage;                        ///< Error message if result is invalid.

        /**
         * @brief Constructs an AnalysisResult with default values.
         */
        AnalysisResult();
        virtual ~AnalysisResult() = default;
    };

    /**
     * @brief Abstract base class for all audio analyzers.
     *
     * Defines the interface for configuring, processing, and retrieving results
     * from an analysis module.
     */
    class Analyzer
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Analyzer() = default;

        /**
         * @brief Configures the analyzer with processing parameters.
         * @param config Analysis configuration settings.
         */
        virtual void Configure(const AnalysisConfig &config) = 0;

        /**
         * @brief Processes a chunk of audio data.
         * @param audioData Span of audio sample data (const float).
         */
        virtual void ProcessBuffer(std::span<const float> audioData) = 0;

        /**
         * @brief Retrieves the latest analysis result.
         * @return Shared pointer to the latest AnalysisResult.
         */
        virtual std::shared_ptr<AnalysisResult> GetLatestResult() const = 0;

        /**
         * @brief Resets the analyzer state.
         */
        virtual void Reset() = 0;

    protected:
        Analyzer() = default;

        Analyzer(const Analyzer &) = default;

        Analyzer(Analyzer &&) noexcept = default;

        Analyzer &operator=(const Analyzer &) = default;

        Analyzer &operator=(Analyzer &&) noexcept = default;
    };

} // namespace GuitarDiagnostics::Analysis
