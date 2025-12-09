#pragma once

#include "UI/Panel.h"

#include <string>

namespace GuitarDiagnostics::Analysis
{
    class AnalysisEngine;
}

namespace GuitarDiagnostics::UI
{

    class StringHealthPanel : public Panel
    {
    public:
        explicit StringHealthPanel(Analysis::AnalysisEngine *engine);
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
        Analysis::AnalysisEngine *analysisEngine;
        std::string panelName;
        bool isActive;
    };

} // namespace GuitarDiagnostics::UI
