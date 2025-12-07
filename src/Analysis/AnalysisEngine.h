#pragma once

#include "Util/LockFreeRingBuffer.h"
#include "Analysis/Analyzer.h"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    class AnalysisEngine
    {
    public:
        AnalysisEngine(Util::LockFreeRingBuffer<float> *ringBuffer, const AnalysisConfig &config);
        ~AnalysisEngine();

        AnalysisEngine(const AnalysisEngine &) = delete;
        AnalysisEngine &operator=(const AnalysisEngine &) = delete;
        AnalysisEngine(AnalysisEngine &&) = delete;
        AnalysisEngine &operator=(AnalysisEngine &&) = delete;

        bool Start();
        void Stop();
        bool IsRunning() const;

        void RegisterAnalyzer(std::shared_ptr<Analyzer> analyzer);
        void Reset();

    private:
        void WorkerThreadFunction();

        Util::LockFreeRingBuffer<float> *ringBuffer;
        AnalysisConfig config;
        std::vector<std::shared_ptr<Analyzer>> analyzers;
        std::vector<float> processingBuffer;
        std::atomic<bool> running;
        std::thread workerThread;
    };

} // namespace GuitarDiagnostics::Analysis
