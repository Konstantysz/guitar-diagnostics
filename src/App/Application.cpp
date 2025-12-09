#include "App/Application.h"

#include "Analysis/Fretbuzz/FretBuzzDetector.h"
#include "Analysis/Intonation/IntonationAnalyzer.h"
#include "Analysis/StringHealth/StringHealthAnalyzer.h"
#include "App/AudioProcessingLayer.h"
#include "App/DiagnosticVisualizationLayer.h"
#include "Util/LockFreeRingBuffer.h"
#include "Analysis/AnalysisEngine.h"

#include <Logger.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdexcept>

namespace GuitarDiagnostics::App
{

    Application::Application()
        : Kappa::Application(GetApplicationSpec()),
          ringBuffer(std::make_unique<Util::LockFreeRingBuffer<float>>(g_kRingBufferCapacity)),
          audioLayer(std::make_unique<AudioProcessingLayer>(ringBuffer.get())),
          analysisEngine(std::make_unique<Analysis::AnalysisEngine>(ringBuffer.get(),
              Analysis::AnalysisConfig(g_kSampleRate, g_kBufferSize)))
    {
        LOG_INFO("Initializing Guitar Diagnostic Analyzer");

        analysisEngine->RegisterAnalyzer(std::make_shared<Analysis::FretBuzzDetector>());
        analysisEngine->RegisterAnalyzer(std::make_shared<Analysis::IntonationAnalyzer>());
        analysisEngine->RegisterAnalyzer(std::make_shared<Analysis::StringHealthAnalyzer>());

        if (!audioLayer->InitializeDefault(g_kSampleRate, g_kBufferSize))
        {
            LOG_ERROR("Failed to initialize audio layer");
            throw std::runtime_error("Audio initialization failed");
        }

        if (!analysisEngine->Start())
        {
            LOG_ERROR("Failed to start analysis engine");
            throw std::runtime_error("Analysis engine start failed");
        }

        if (!audioLayer->Start())
        {
            LOG_ERROR("Failed to start audio processing");
            throw std::runtime_error("Audio start failed");
        }

        InitializeImGui();

        PushLayer<DiagnosticVisualizationLayer>(analysisEngine.get(), ringBuffer.get());

        LOG_INFO("Application initialized successfully");
    }

    Application::~Application()
    {
        LOG_INFO("Shutting down Guitar Diagnostic Analyzer");

        ShutdownImGui();

        audioLayer->Stop();
        analysisEngine->Stop();
        audioLayer->Shutdown();

        LOG_INFO("Application shutdown complete");
    }

    Kappa::ApplicationSpecification Application::GetApplicationSpec()
    {
        Kappa::ApplicationSpecification spec;
        spec.name = "Guitar Diagnostic Analyzer";
        spec.windowSpecification.title = "Guitar Diagnostic Analyzer";
        spec.windowSpecification.width = 1280;
        spec.windowSpecification.height = 720;
        return spec;
    }

    void Application::InitializeImGui()
    {
        LOG_INFO("Initializing ImGui");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        GLFWwindow *window = GetWindow().GetHandle();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        LOG_INFO("ImGui initialized successfully");
    }

    void Application::ShutdownImGui()
    {
        LOG_INFO("Shutting down ImGui");

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Application::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

} // namespace GuitarDiagnostics::App
