#include "Analysis/AnalysisEngine.h"

#include <algorithm>

namespace GuitarDiagnostics::Analysis
{

    AnalysisEngine::AnalysisEngine(Util::LockFreeRingBuffer<float> *ringBuffer, const AnalysisConfig &config)
        : ringBuffer(ringBuffer), config(config), analyzers(), processingBuffer(config.bufferSize), running(false),
          workerThread()
    {
    }

    AnalysisEngine::~AnalysisEngine()
    {
        Stop();
    }

    bool AnalysisEngine::Start()
    {
        bool expected = false;
        if (!running.compare_exchange_strong(expected, true))
        {
            return false;
        }

        workerThread = std::thread(&AnalysisEngine::WorkerThreadFunction, this);
        return true;
    }

    void AnalysisEngine::Stop()
    {
        running.store(false);

        if (workerThread.joinable())
        {
            workerThread.join();
        }
    }

    bool AnalysisEngine::IsRunning() const
    {
        return running.load();
    }

    void AnalysisEngine::RegisterAnalyzer(std::shared_ptr<Analyzer> analyzer)
    {
        if (analyzer)
        {
            analyzer->Configure(config);
            analyzers.push_back(analyzer);
        }
    }

    void AnalysisEngine::Reset()
    {
        for (auto &analyzer : analyzers)
        {
            analyzer->Reset();
        }
    }

    void AnalysisEngine::WorkerThreadFunction()
    {
        std::span<float> bufferSpan(processingBuffer.data(), processingBuffer.size());

        while (running.load())
        {
            size_t available = ringBuffer->GetAvailableRead();

            if (available >= config.bufferSize)
            {
                size_t samplesRead = ringBuffer->Read(bufferSpan);

                if (samplesRead > 0)
                {
                    std::span<const float> audioData(processingBuffer.data(), samplesRead);

                    for (auto &analyzer : analyzers)
                    {
                        analyzer->ProcessBuffer(audioData);
                    }
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }

} // namespace GuitarDiagnostics::Analysis
