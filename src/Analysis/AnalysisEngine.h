#pragma once

#include "Util/LockFreeRingBuffer.h"
#include "Analysis/Analyzer.h"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace GuitarDiagnostics::Analysis
{

    /**
     * @brief Core engine managing multiple analyzers and the analysis thread.
     *
     * Handles audio data buffering from the ring buffer and distributes it
     * to registered analyzers in a dedicated worker thread.
     */
    class AnalysisEngine
    {
    public:
        /**
         * @brief Constructs the AnalysisEngine.
         * @param ringBuffer Pointer to the ring buffer containing audio data.
         * @param config Initial analysis configuration.
         */
        AnalysisEngine(Util::LockFreeRingBuffer<float> *ringBuffer, const AnalysisConfig &config);

        /**
         * @brief Destructor. Stops the worker thread if running.
         */
        ~AnalysisEngine();

        AnalysisEngine(const AnalysisEngine &) = delete;

        AnalysisEngine &operator=(const AnalysisEngine &) = delete;

        AnalysisEngine(AnalysisEngine &&) = delete;

        AnalysisEngine &operator=(AnalysisEngine &&) = delete;

        /**
         * @brief Starts the analysis worker thread.
         * @return True if started successfully, false otherwise.
         */
        bool Start();

        /**
         * @brief Stops the analysis worker thread.
         */
        void Stop();

        /**
         * @brief Checks if the analysis engine is running.
         * @return True if running, false otherwise.
         */
        bool IsRunning() const;

        /**
         * @brief Registers an analyzer with the engine.
         * @param analyzer Shared pointer to the analyzer to register.
         */
        void RegisterAnalyzer(std::shared_ptr<Analyzer> analyzer);

        /**
         * @brief Resets all registered analyzers.
         */
        void Reset();

        /**
         * @brief Retrieves a registered analyzer by type.
         * @tparam T The type of analyzer to retrieve.
         * @return Shared pointer to the analyzer if found, nullptr otherwise.
         */
        template<typename T> std::shared_ptr<T> GetAnalyzer() const
        {
            for (const auto &analyzer : analyzers)
            {
                auto typedAnalyzer = std::dynamic_pointer_cast<T>(analyzer);
                if (typedAnalyzer)
                {
                    return typedAnalyzer;
                }
            }
            return nullptr;
        }

    private:
        /**
         * @brief Main loop for the worker thread.
         *
         * Consumes data from the ring buffer and feeds it to analyzers.
         */
        void WorkerThreadFunction();

        Util::LockFreeRingBuffer<float> *ringBuffer;      ///< Pointer to the ring buffer.
        AnalysisConfig config;                            ///< Current analysis configuration.
        std::vector<std::shared_ptr<Analyzer>> analyzers; ///< List of registered analyzers.
        std::vector<float> processingBuffer;              ///< Internal buffer for processing audio chunks.
        std::atomic<bool> running;                        ///< Atomic flag indicating if the engine is running.
        std::thread workerThread;                         ///< The worker thread instance.
    };

} // namespace GuitarDiagnostics::Analysis
