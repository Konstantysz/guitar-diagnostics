#pragma once

#include "UI/Panel.h"
#include "Analysis/AnalysisEngine.h"

#include <string>

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Panel for Intonation analysis visualization.
     *
     * Guides the user through the intonation check process and displays results.
     */
    class IntonationPanel : public Panel
    {
    public:
        /**
         * @brief Constructs the IntonationPanel.
         * @param engine Pointer to the analysis engine.
         */
        explicit IntonationPanel(Analysis::AnalysisEngine *engine);

        /**
         * @brief Destructor.
         */
        ~IntonationPanel() override;

        IntonationPanel(const IntonationPanel &) = delete;

        IntonationPanel &operator=(const IntonationPanel &) = delete;

        IntonationPanel(IntonationPanel &&) = delete;

        IntonationPanel &operator=(IntonationPanel &&) = delete;

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
