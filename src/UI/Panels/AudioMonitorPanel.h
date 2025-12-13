#pragma once

#include "UI/Panel.h"
#include "Analysis/AnalysisEngine.h"

#include <string>
#include <vector>

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Panel for monitoring raw audio input.
     *
     * Displays a real-time waveform and RMS level of the input signal.
     */
    class AudioMonitorPanel : public Panel
    {
    public:
        /**
         * @brief Constructs the AudioMonitorPanel.
         * @param ringBuffer Pointer to the audio ring buffer.
         */
        explicit AudioMonitorPanel(Util::LockFreeRingBuffer<float> *ringBuffer);

        /**
         * @brief Destructor.
         */
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
        /**
         * @brief Calculates RMS value of the current buffer.
         * @param buffer Input audio buffer.
         * @return Root Mean Square value.
         */
        float CalculateRMS(const std::vector<float> &buffer);

        Util::LockFreeRingBuffer<float> *ringBuffer; ///< Pointer to the audio ring buffer.
        std::string panelName;                       ///< Display name of the panel.
        bool isActive;                               ///< Active state flag.
        std::vector<float> waveformBuffer;           ///< Buffer for visualization data.
        float currentRMS;                            ///< Current RMS value.

        static constexpr size_t g_kWaveformSize = 512; ///< Size of the waveform display buffer.
    };

} // namespace GuitarDiagnostics::UI
