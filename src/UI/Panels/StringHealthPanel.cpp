#include "UI/Panels/StringHealthPanel.h"

#include "Analysis/StringHealth/StringHealthAnalyzer.h"
#include "Analysis/AnalysisEngine.h"

#include <imgui.h>

namespace GuitarDiagnostics::UI
{

    StringHealthPanel::StringHealthPanel(Analysis::AnalysisEngine *engine)
        : analysisEngine(engine), panelName("String Health"), isActive(false)
    {
    }

    StringHealthPanel::~StringHealthPanel()
    {
    }

    void StringHealthPanel::OnAttach()
    {
    }

    void StringHealthPanel::OnDetach()
    {
    }

    void StringHealthPanel::OnUpdate([[maybe_unused]] float deltaTime)
    {
    }

    void StringHealthPanel::OnImGuiRender()
    {
        auto analyzer = analysisEngine->GetAnalyzer<Analysis::StringHealthAnalyzer>();
        if (!analyzer)
        {
            ImGui::Text("Error: StringHealthAnalyzer not initialized");
            return;
        }

        auto baseResult = analyzer->GetLatestResult();
        auto result = std::dynamic_pointer_cast<Analysis::StringHealthResult>(baseResult);

        if (!result || !result->isValid)
        {
            ImGui::Text("Waiting for analysis data...");
            ImGui::Text("");
            ImGui::TextWrapped("Play a sustained note on your guitar to analyze string health.");
            return;
        }

        ImGui::Text("String Health Score:");

        ImVec4 color;
        if (result->healthScore > 0.7f)
        {
            color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        }
        else if (result->healthScore > 0.4f)
        {
            color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        }
        else
        {
            color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
        ImGui::ProgressBar(result->healthScore, ImVec2(-1, 0));
        ImGui::PopStyleColor();

        if (result->healthScore > 0.7f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "HEALTHY");
        }
        else if (result->healthScore > 0.4f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "DEGRADED");
        }
        else
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "POOR");
        }

        ImGui::Separator();

        ImGui::Text("Analysis Details:");
        ImGui::Indent();
        ImGui::BulletText("Decay Rate: %.2f dB/s", result->decayRate);
        ImGui::BulletText("Spectral Centroid: %.2f Hz", result->spectralCentroid);
        ImGui::BulletText("Inharmonicity: %.4f", result->inharmonicity);
        ImGui::BulletText("Fundamental Frequency: %.2f Hz", result->fundamentalFrequency);
        ImGui::Unindent();
    }

    const std::string &StringHealthPanel::GetName() const
    {
        return panelName;
    }

    bool StringHealthPanel::IsActive() const
    {
        return isActive;
    }

    void StringHealthPanel::SetActive(bool active)
    {
        isActive = active;
    }

} // namespace GuitarDiagnostics::UI
