#pragma once

#include "UI/Panel.h"

#include <string>
#include <vector>

namespace GuitarDiagnostics::Util
{
    template<typename T> class LockFreeRingBuffer;
}

namespace GuitarDiagnostics::UI
{

    class AudioMonitorPanel : public Panel
    {
    public:
        explicit AudioMonitorPanel(Util::LockFreeRingBuffer<float> *ringBuffer);
        ~AudioMonitorPanel() override;

        AudioMonitorPanel(const AudioMonitorPanel &) = delete;
        AudioMonitorPanel &operator=(const AudioMonitorPanel &) = delete;
        AudioMonitorPanel(AudioMonitorPanel &&) = delete;
        AudioMonitorPanel &operator=(AudioMonitorPanel &&) = delete;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;

        const std::string &GetName() const override;
        bool IsActive() const override;
        void SetActive(bool active) override;

    private:
        float CalculateRMS(const std::vector<float> &buffer);

        Util::LockFreeRingBuffer<float> *ringBuffer;
        std::string panelName;
        bool isActive;
        std::vector<float> waveformBuffer;
        float currentRMS;

        static constexpr size_t g_kWaveformSize = 512;
    };

} // namespace GuitarDiagnostics::UI
