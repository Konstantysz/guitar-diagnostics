#include "UI/Panels/IntonationPanel.h"

#include "Analysis/Intonation/IntonationAnalyzer.h"
#include "Analysis/AnalysisEngine.h"

#include <imgui.h>

#include <cmath>
#include <string>

namespace GuitarDiagnostics::UI
{

    IntonationPanel::IntonationPanel(Analysis::AnalysisEngine *engine)
        : analysisEngine(engine), panelName("Intonation"), isActive(false)
    {
    }

    IntonationPanel::~IntonationPanel()
    {
    }

    void IntonationPanel::OnAttach()
    {
    }

    void IntonationPanel::OnDetach()
    {
    }

    void IntonationPanel::OnUpdate([[maybe_unused]] float deltaTime)
    {
    }

    void IntonationPanel::OnImGuiRender()
    {
        auto analyzer = analysisEngine->GetAnalyzer<Analysis::IntonationAnalyzer>();
        if (!analyzer)
        {
            ImGui::Text("Error: IntonationAnalyzer not initialized");
            return;
        }

        auto baseResult = analyzer->GetLatestResult();
        auto result = std::dynamic_pointer_cast<Analysis::IntonationResult>(baseResult);

        if (!result || !result->isValid)
        {
            ImGui::Text("Waiting for analysis data...");
            ImGui::Text("");
            ImGui::TextWrapped("Instructions:");
            ImGui::BulletText("1. Play an open string");
            ImGui::BulletText("2. Wait for stable pitch detection");
            ImGui::BulletText("3. Play the same string at the 12th fret");
            ImGui::BulletText("4. Check the intonation deviation");
            return;
        }

        ImGui::Text("Analysis State:");
        const char *stateStr = "Unknown";
        switch (result->state)
        {
        case Analysis::IntonationState::Idle:
            stateStr = "Idle - Waiting for input";
            break;
        case Analysis::IntonationState::OpenString:
            stateStr = "Detecting open string pitch";
            break;
        case Analysis::IntonationState::WaitFor12thFret:
            stateStr = "Waiting for 12th fret note";
            break;
        case Analysis::IntonationState::FrettedString:
            stateStr = "Detecting fretted pitch";
            break;
        case Analysis::IntonationState::Complete:
            stateStr = "Analysis complete";
            break;
        }

        float progressValue = static_cast<float>(static_cast<int>(result->state)) / 4.0f;
        ImGui::ProgressBar(progressValue, ImVec2(-1, 0), stateStr);

        ImGui::Separator();

        if (result->state == Analysis::IntonationState::Complete)
        {
            ImGui::Text("Frequency Analysis:");
            ImGui::Indent();
            ImGui::BulletText("Open String: %.2f Hz", result->openStringFrequency);
            ImGui::BulletText("12th Fret: %.2f Hz", result->frettedStringFrequency);
            ImGui::BulletText("Expected: %.2f Hz", result->expectedFrettedFrequency);
            ImGui::Unindent();

            ImGui::Separator();

            ImGui::Text("Intonation Deviation:");
            float deviation = result->centDeviation;

            ImVec4 color = result->isInTune ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
            std::string centsLabel = std::to_string(static_cast<int>(std::round(deviation))) + " cents";
            float normalizedValue = (deviation + 50.0f) / 100.0f;
            normalizedValue = normalizedValue < 0.0f ? 0.0f : (normalizedValue > 1.0f ? 1.0f : normalizedValue);
            ImGui::ProgressBar(normalizedValue, ImVec2(-1, 0), centsLabel.c_str());
            ImGui::PopStyleColor();

            if (result->isInTune)
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "IN TUNE");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OUT OF TUNE");
            }
        }
        else
        {
            ImGui::TextWrapped("Follow the instructions above to complete the intonation analysis.");
        }
    }

    const std::string &IntonationPanel::GetName() const
    {
        return panelName;
    }

    bool IntonationPanel::IsActive() const
    {
        return isActive;
    }

    void IntonationPanel::SetActive(bool active)
    {
        isActive = active;
    }

} // namespace GuitarDiagnostics::UI
