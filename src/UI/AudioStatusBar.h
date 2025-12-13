#pragma once

#include "App/AudioProcessingLayer.h"

namespace GuitarDiagnostics::UI
{

    /**
     * @brief Persistent header bar showing audio status and help toggle.
     *
     * Displays above tab panels with device selector, input level meter,
     * and help mode toggle. Thread-safe: reads atomic peak level from audio layer.
     */
    class AudioStatusBar
    {
    public:
        /**
         * @brief Constructs the AudioStatusBar.
         * @param audioLayer Pointer to the audio processing layer for reading peak level.
         */
        AudioStatusBar(App::AudioProcessingLayer *audioLayer);

        /**
         * @brief Destructor.
         */
        ~AudioStatusBar();

        AudioStatusBar(const AudioStatusBar &) = delete;

        AudioStatusBar &operator=(const AudioStatusBar &) = delete;

        AudioStatusBar(AudioStatusBar &&) = delete;

        AudioStatusBar &operator=(AudioStatusBar &&) = delete;

        /**
         * @brief Renders the status bar ImGui elements.
         */
        void OnImGuiRender();

        /**
         * @brief Checks if help mode is enabled.
         * @return True if help mode is active, false otherwise.
         */
        bool IsHelpModeEnabled() const;

        /**
         * @brief Sets the help mode state.
         * @param enabled True to enable help mode, false to disable.
         */
        void SetHelpMode(bool enabled);

    private:
        App::AudioProcessingLayer *audioLayer; ///< Pointer to audio layer for peak level reading.
        bool helpModeEnabled;                  ///< Current state of help mode toggle.
        float smoothedPeakLevel;               ///< Smoothed peak level for UI display.
    };

} // namespace GuitarDiagnostics::UI
