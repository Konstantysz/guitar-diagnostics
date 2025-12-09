#pragma once

#include "UI/Panel.h"

#include <string>

namespace GuitarDiagnostics::Analysis
{
    class AnalysisEngine;
}

namespace GuitarDiagnostics::UI
{

    class IntonationPanel : public Panel
    {
    public:
        explicit IntonationPanel(Analysis::AnalysisEngine *engine);
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
        Analysis::AnalysisEngine *analysisEngine;
        std::string panelName;
        bool isActive;
    };

} // namespace GuitarDiagnostics::UI
