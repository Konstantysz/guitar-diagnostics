#include "Analysis/Analyzer.h"

namespace GuitarDiagnostics::Analysis
{

    AnalysisConfig::AnalysisConfig(float sampleRate, uint32_t bufferSize)
        : sampleRate(sampleRate), bufferSize(bufferSize)
    {
    }

    AnalysisResult::AnalysisResult() : timestamp(std::chrono::system_clock::now()), isValid(false), errorMessage()
    {
    }

} // namespace GuitarDiagnostics::Analysis
