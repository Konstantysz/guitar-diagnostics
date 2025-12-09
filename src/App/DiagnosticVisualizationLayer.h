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

    /**
     * @brief Layer responsible for visualizing analysis results.
     *
     * Renders the UI panels for different diagnostic tools (Fret Buzz, Intonation, String Health)
     * and visualization of raw audio data.
     */
    class DiagnosticVisualizationLayer : public Kappa::Layer
    {
    public:
        /**
         * @brief Constructs the DiagnosticVisualizationLayer.
         * @param engine Pointer to the analysis engine providing data.
         * @param ringBuffer Pointer to the ring buffer for audio monitoring visualization.
         */
        DiagnosticVisualizationLayer(Analysis::AnalysisEngine *engine, Util::LockFreeRingBuffer<float> *ringBuffer);

        /**
         * @brief Destructor.
         */
        ~DiagnosticVisualizationLayer() override;

        DiagnosticVisualizationLayer(const DiagnosticVisualizationLayer &) = delete;

        DiagnosticVisualizationLayer &operator=(const DiagnosticVisualizationLayer &) = delete;

        DiagnosticVisualizationLayer(DiagnosticVisualizationLayer &&) = delete;

        DiagnosticVisualizationLayer &operator=(DiagnosticVisualizationLayer &&) = delete;

        /**
         * @brief Specific update logic for this layer.
         * @param deltaTime Time elapsed since the last frame.
         */
        void OnUpdate(float deltaTime) override;

        /**
         * @brief Renders the layer's UI elements.
         */
        void OnRender() override;

        /**
         * @brief Handles events propagated to this layer.
         * @param event The event to handle.
         */
        void OnEvent(Kappa::Event &event) override;

    private:
        Analysis::AnalysisEngine *analysisEngine;         ///< Pointer to the analysis engine.
        std::unique_ptr<UI::TabController> tabController; ///< Controller for managing UI tabs.
    };

} // namespace GuitarDiagnostics::App
