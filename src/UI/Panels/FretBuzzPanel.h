#pragma once

#include "UI/Panel.h"

#include <string>

namespace GuitarDiagnostics::Analysis
{
    class AnalysisEngine;
}

namespace GuitarDiagnostics::UI
{

    class FretBuzzPanel : public Panel
    {
    public:
        explicit FretBuzzPanel(Analysis::AnalysisEngine *engine);
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
        Analysis::AnalysisEngine *analysisEngine;
        std::string panelName;
        bool isActive;
    };

} // namespace GuitarDiagnostics::UI
