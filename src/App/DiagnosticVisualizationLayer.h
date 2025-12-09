#pragma once

#include <Layer.h>

#include <memory>

namespace GuitarDiagnostics::Analysis
{
    class AnalysisEngine;
}

namespace GuitarDiagnostics::Util
{
    template<typename T> class LockFreeRingBuffer;
}

namespace GuitarDiagnostics::UI
{
    class TabController;
}

namespace GuitarDiagnostics::App
{

    class DiagnosticVisualizationLayer : public Kappa::Layer
    {
    public:
        DiagnosticVisualizationLayer(Analysis::AnalysisEngine *engine, Util::LockFreeRingBuffer<float> *ringBuffer);
        ~DiagnosticVisualizationLayer() override;

        DiagnosticVisualizationLayer(const DiagnosticVisualizationLayer &) = delete;
        DiagnosticVisualizationLayer &operator=(const DiagnosticVisualizationLayer &) = delete;
        DiagnosticVisualizationLayer(DiagnosticVisualizationLayer &&) = delete;
        DiagnosticVisualizationLayer &operator=(DiagnosticVisualizationLayer &&) = delete;

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnEvent(Kappa::Event &event) override;

    private:
        Analysis::AnalysisEngine *analysisEngine;
        std::unique_ptr<UI::TabController> tabController;
    };

} // namespace GuitarDiagnostics::App
