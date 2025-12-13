#pragma once

#include "UI/Panel.h"
#include "Analysis/AnalysisEngine.h"

#include <string>

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Panel for String Health analysis visualization.
     *
     * Displays metrics related to string age, decay, and spectral content.
     */
    class StringHealthPanel : public Panel
    {
    public:
        /**
         * @brief Constructs the StringHealthPanel.
         * @param engine Pointer to the analysis engine.
         */
        explicit StringHealthPanel(Analysis::AnalysisEngine *engine);

        /**
         * @brief Destructor.
         */
        ~StringHealthPanel() override;

        StringHealthPanel(const StringHealthPanel &) = delete;

        StringHealthPanel &operator=(const StringHealthPanel &) = delete;

        StringHealthPanel(StringHealthPanel &&) = delete;

        StringHealthPanel &operator=(StringHealthPanel &&) = delete;

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
