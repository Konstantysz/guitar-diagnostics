#include "App/DiagnosticVisualizationLayer.h"

#include "App/AudioProcessingLayer.h"
#include "UI/AudioStatusBar.h"
#include "UI/Panels/AudioMonitorPanel.h"
#include "UI/Panels/FretBuzzPanel.h"
#include "UI/Panels/IntonationPanel.h"
#include "UI/Panels/StringHealthPanel.h"
#include "UI/TabController.h"
#include "Util/LockFreeRingBuffer.h"
#include "Analysis/AnalysisEngine.h"

#include <Logger.h>

namespace GuitarDiagnostics::App
{

    DiagnosticVisualizationLayer::DiagnosticVisualizationLayer(Analysis::AnalysisEngine *engine,
        Util::LockFreeRingBuffer<float> *ringBuffer,
        AudioProcessingLayer *audioLayer)
        : analysisEngine(engine), tabController(nullptr), audioStatusBar(nullptr)
    {
        LOG_INFO("Initializing DiagnosticVisualizationLayer");

        audioStatusBar = std::make_unique<UI::AudioStatusBar>(audioLayer);

        auto fretBuzzPanel = std::make_unique<UI::FretBuzzPanel>(analysisEngine);
        auto intonationPanel = std::make_unique<UI::IntonationPanel>(analysisEngine);
        auto stringHealthPanel = std::make_unique<UI::StringHealthPanel>(analysisEngine);
        auto audioMonitorPanel = std::make_unique<UI::AudioMonitorPanel>(ringBuffer);

        tabController = std::make_unique<UI::TabController>(std::move(fretBuzzPanel),
            std::move(intonationPanel),
            std::move(stringHealthPanel),
            std::move(audioMonitorPanel));

        tabController->OnAttach();

        LOG_INFO("DiagnosticVisualizationLayer initialized");
    }

    DiagnosticVisualizationLayer::~DiagnosticVisualizationLayer()
    {
        LOG_INFO("Destroying DiagnosticVisualizationLayer");

        if (tabController)
        {
            tabController->OnDetach();
        }
    }

    void DiagnosticVisualizationLayer::OnUpdate(float deltaTime)
    {
        if (tabController)
        {
            tabController->OnUpdate(deltaTime);
        }
    }

    void DiagnosticVisualizationLayer::OnRender()
    {
        if (audioStatusBar)
        {
            audioStatusBar->OnImGuiRender();
        }

        if (tabController)
        {
            tabController->Render();
        }
    }

    void DiagnosticVisualizationLayer::OnEvent([[maybe_unused]] Kappa::Event &event)
    {
    }

} // namespace GuitarDiagnostics::App
