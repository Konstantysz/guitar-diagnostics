#include "UI/Panels/AudioMonitorPanel.h"

#include "Util/LockFreeRingBuffer.h"

#include <imgui.h>

#include <algorithm>
#include <cmath>
#include <span>

namespace GuitarDiagnostics::UI
{

    AudioMonitorPanel::AudioMonitorPanel(Util::LockFreeRingBuffer<float> *ringBuffer)
        : ringBuffer(ringBuffer), panelName("Audio Monitor"), isActive(false), waveformBuffer(), currentRMS(0.0f)
    {
        waveformBuffer.reserve(g_kWaveformSize);
    }

    AudioMonitorPanel::~AudioMonitorPanel()
    {
    }

    void AudioMonitorPanel::OnAttach()
    {
    }

    void AudioMonitorPanel::OnDetach()
    {
    }

    void AudioMonitorPanel::OnUpdate([[maybe_unused]] float deltaTime)
    {
        size_t available = ringBuffer->GetAvailableRead();

        if (available >= g_kWaveformSize)
        {
            waveformBuffer.resize(g_kWaveformSize);
            size_t samplesRead = ringBuffer->Read(std::span<float>(waveformBuffer.data(), g_kWaveformSize));

            if (samplesRead == g_kWaveformSize)
            {
                currentRMS = CalculateRMS(waveformBuffer);
            }
        }
    }

    void AudioMonitorPanel::OnImGuiRender()
    {
        ImGui::Text("Audio Input Monitor");
        ImGui::Separator();

        if (waveformBuffer.empty())
        {
            ImGui::Text("Waiting for audio data...");
            return;
        }

        ImGui::Text("Waveform:");
        ImGui::PlotLines("##waveform",
            waveformBuffer.data(),
            static_cast<int>(waveformBuffer.size()),
            0,
            nullptr,
            -1.0f,
            1.0f,
            ImVec2(ImGui::GetContentRegionAvail().x, 200.0f));

        ImGui::Separator();

        ImGui::Text("RMS Level:");
        ImGui::ProgressBar(currentRMS, ImVec2(-1, 0));

        ImGui::Text("RMS: %.4f", currentRMS);

        float dbLevel = 20.0f * std::log10(currentRMS + 1e-10f);
        ImGui::Text("Level: %.2f dB", dbLevel);
    }

    const std::string &AudioMonitorPanel::GetName() const
    {
        return panelName;
    }

    bool AudioMonitorPanel::IsActive() const
    {
        return isActive;
    }

    void AudioMonitorPanel::SetActive(bool active)
    {
        isActive = active;
    }

    float AudioMonitorPanel::CalculateRMS(const std::vector<float> &buffer)
    {
        if (buffer.empty())
        {
            return 0.0f;
        }

        float sumSquares = 0.0f;
        for (float sample : buffer)
        {
            sumSquares += sample * sample;
        }

        return std::sqrt(sumSquares / static_cast<float>(buffer.size()));
    }

} // namespace GuitarDiagnostics::UI
