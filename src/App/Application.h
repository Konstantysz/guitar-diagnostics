#pragma once

#include <Application.h>

#include <cstdint>
#include <memory>

namespace GuitarDiagnostics::App
{
    class AudioProcessingLayer;
}

namespace GuitarDiagnostics::Analysis
{
    class AnalysisEngine;
}

namespace GuitarDiagnostics::Util
{
    template<typename T> class LockFreeRingBuffer;
}

namespace GuitarDiagnostics::App
{

    /**
     * @brief Main application class inheriting from Kappa::Application.
     *
     * Handles the initialization and management of the audio layer, analysis engine,
     * and UI integration through ImGui.
     */
    class Application : public Kappa::Application
    {
    public:
        /**
         * @brief Constructs the Application instance.
         */
        Application();

        /**
         * @brief Destructor.
         */
        ~Application() override;

        Application(const Application &) = delete;

        Application &operator=(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(Application &&) = delete;

    protected:
        /**
         * @brief Called at the beginning of each frame.
         *
         * Setup frame-specific resources or states.
         */
        void BeginFrame() override;

        /**
         * @brief Called at the end of each frame.
         *
         * Clean up frame-specific resources or states.
         */
        void EndFrame() override;

    private:
        /**
         * @brief Retrieves the application specification.
         * @return ApplicationSpecification struct with app settings.
         */
        static Kappa::ApplicationSpecification GetApplicationSpec();

        /**
         * @brief Initializes ImGui context and style.
         */
        void InitializeImGui();

        /**
         * @brief Shuts down ImGui context.
         */
        void ShutdownImGui();

        std::unique_ptr<Util::LockFreeRingBuffer<float>>
            ringBuffer;                                   ///< Ring buffer for audio data transfer between threads.
        std::unique_ptr<AudioProcessingLayer> audioLayer; ///< Handles platform-specific audio device interaction.
        std::unique_ptr<Analysis::AnalysisEngine> analysisEngine; ///< Core engine for analyzing audio data.

        static constexpr float g_kSampleRate = 48000.0f;       ///< Audio sample rate in Hz.
        static constexpr uint32_t g_kBufferSize = 512;         ///< Audio buffer size.
        static constexpr size_t g_kRingBufferCapacity = 16384; ///< Capacity of the ring buffer.
    };

} // namespace GuitarDiagnostics::App
