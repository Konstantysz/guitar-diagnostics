#include "UI/AudioStatusBar.h"

#include <imgui.h>

#include <algorithm>

namespace GuitarDiagnostics::UI
{

    AudioStatusBar::AudioStatusBar(App::AudioProcessingLayer *audioLayer)
        : audioLayer(audioLayer), helpModeEnabled(false), smoothedPeakLevel(0.0f)
    {
    }

    AudioStatusBar::~AudioStatusBar()
    {
    }

    void AudioStatusBar::OnImGuiRender()
    {
        // Create fixed-height header bar (50px)
        ImGui::BeginChild("AudioStatusBar", ImVec2(0, 50), true, ImGuiWindowFlags_NoScrollbar);

        // Left section: Audio device info
        ImGui::Text("Audio Device:");
        ImGui::SameLine();

        if (audioLayer && audioLayer->IsRunning())
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Running");
        }
        else if (audioLayer && audioLayer->IsOpen())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Open (Stopped)");
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Initialized");
        }

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        // Middle section: Input level meter
        ImGui::Text("Input Level:");
        ImGui::SameLine();

        // Read peak level from audio layer (thread-safe atomic read)
        float currentPeak = 0.0f;
        if (audioLayer)
        {
            currentPeak = audioLayer->GetPeakInputLevel();
        }

        // Smooth the peak level for display (60 FPS UI, avoid jitter)
        const float smoothingFactor = 0.3f;
        smoothedPeakLevel = smoothingFactor * currentPeak + (1.0f - smoothingFactor) * smoothedPeakLevel;

        // Clamp to [0, 1]
        float displayLevel = std::clamp(smoothedPeakLevel, 0.0f, 1.0f);

        // Draw level meter as progress bar
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
        ImGui::ProgressBar(displayLevel, ImVec2(200.0f, 20.0f), "");
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::Text("%.2f", displayLevel);

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        // Right section: Help mode toggle
        ImGui::Text("Help Mode:");
        ImGui::SameLine();

        if (ImGui::Checkbox("##HelpToggle", &helpModeEnabled))
        {
            // Toggle state changed (user can read via IsHelpModeEnabled)
        }

        ImGui::EndChild();
    }

    bool AudioStatusBar::IsHelpModeEnabled() const
    {
        return helpModeEnabled;
    }

    void AudioStatusBar::SetHelpMode(bool enabled)
    {
        helpModeEnabled = enabled;
    }

} // namespace GuitarDiagnostics::UI
