#pragma once

#include "UI/Panel.h"

#include <string>

namespace GuitarDiagnostics::Analysis
{
    class AnalysisEngine;
}

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Panel for Fret Buzz analysis visualization.
     *
     * Displays buzz score, transient indicators, and other fret buzz metrics.
     */
    class FretBuzzPanel : public Panel
    {
    public:
        /**
         * @brief Constructs the FretBuzzPanel.
         * @param engine Pointer to the analysis engine.
         */
        explicit FretBuzzPanel(Analysis::AnalysisEngine *engine);

        /**
         * @brief Destructor.
         */
        ~FretBuzzPanel() override;

        FretBuzzPanel(const FretBuzzPanel &) = delete;

        FretBuzzPanel &operator=(const FretBuzzPanel &) = delete;

        FretBuzzPanel(FretBuzzPanel &&) = delete;

        FretBuzzPanel &operator=(FretBuzzPanel &&) = delete;

        void OnAttach() override;

        void OnDetach() override;

        void OnUpdate(float deltaTime) override;

        void OnImGuiRender() override;

        const std::string &GetName() const override;

        bool IsActive() const override;

        void SetActive(bool active) override;

    private:
        Analysis::AnalysisEngine *analysisEngine; ///< Pointer to the analysis engine.
        std::string panelName;                    ///< Display name.
        bool isActive;                            ///< Active state.
    };

} // namespace GuitarDiagnostics::UI
