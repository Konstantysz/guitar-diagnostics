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

    class Application : public Kappa::Application
    {
    public:
        Application();
        ~Application() override;

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;
        Application(Application &&) = delete;
        Application &operator=(Application &&) = delete;

    protected:
        void BeginFrame() override;
        void EndFrame() override;

    private:
        static Kappa::ApplicationSpecification GetApplicationSpec();
        void InitializeImGui();
        void ShutdownImGui();

        std::unique_ptr<Util::LockFreeRingBuffer<float>> ringBuffer;
        std::unique_ptr<AudioProcessingLayer> audioLayer;
        std::unique_ptr<Analysis::AnalysisEngine> analysisEngine;

        static constexpr float g_kSampleRate = 48000.0f;
        static constexpr uint32_t g_kBufferSize = 512;
        static constexpr size_t g_kRingBufferCapacity = 16384;
    };

} // namespace GuitarDiagnostics::App
