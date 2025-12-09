#include "UI/Panels/FretBuzzPanel.h"

#include "Analysis/Fretbuzz/FretBuzzDetector.h"
#include "Analysis/AnalysisEngine.h"

#include <imgui.h>

namespace GuitarDiagnostics::UI
{

    FretBuzzPanel::FretBuzzPanel(Analysis::AnalysisEngine *engine)
        : analysisEngine(engine), panelName("Fret Buzz"), isActive(false)
    {
    }

    FretBuzzPanel::~FretBuzzPanel()
    {
    }

    void FretBuzzPanel::OnAttach()
    {
    }

    void FretBuzzPanel::OnDetach()
    {
    }

    void FretBuzzPanel::OnUpdate([[maybe_unused]] float deltaTime)
    {
    }

    void FretBuzzPanel::OnImGuiRender()
    {
        auto detector = analysisEngine->GetAnalyzer<Analysis::FretBuzzDetector>();
        if (!detector)
        {
            ImGui::Text("Error: FretBuzzDetector not initialized");
            return;
        }

        auto baseResult = detector->GetLatestResult();
        auto result = std::dynamic_pointer_cast<Analysis::FretBuzzResult>(baseResult);

        if (!result || !result->isValid)
        {
            ImGui::Text("Waiting for analysis data...");
            ImGui::Text("");
            ImGui::TextWrapped("Play a note on your guitar to begin fret buzz detection.");
            return;
        }

        ImGui::Text("Buzz Score:");
        ImGui::ProgressBar(result->buzzScore, ImVec2(-1, 0));

        if (result->buzzScore > 0.5f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "BUZZ DETECTED");
        }

        ImGui::Separator();

        ImGui::Text("Onset Detected: %s", result->onsetDetected ? "YES" : "NO");

        ImGui::Separator();

        ImGui::Text("Component Scores:");
        ImGui::Indent();
        ImGui::BulletText("Transient: %.2f", result->transientScore);
        ImGui::BulletText("High-Freq Noise: %.2f", result->highFreqEnergyScore);
        ImGui::BulletText("Inharmonicity: %.2f", result->inharmonicityScore);
        ImGui::Unindent();
    }

    const std::string &FretBuzzPanel::GetName() const
    {
        return panelName;
    }

    bool FretBuzzPanel::IsActive() const
    {
        return isActive;
    }

    void FretBuzzPanel::SetActive(bool active)
    {
        isActive = active;
    }

} // namespace GuitarDiagnostics::UI
