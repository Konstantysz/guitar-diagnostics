#pragma once

#include "Audio/AudioDeviceManager.h"
#include "Util/LockFreeRingBuffer.h"

#include <RtAudioDevice.h>

#include <cstdint>
#include <memory>

namespace GuitarDiagnostics::App
{

    /**
     * @brief Manages audio input and processing.
     *
     * Handles audio device initialization, streaming, and buffering of input data.
     */
    class AudioProcessingLayer
    {
    public:
        /**
         * @brief Constructs the AudioProcessingLayer.
         * @param ringBuffer Pointer to the ring buffer for storing audio samples.
         */
        AudioProcessingLayer(Util::LockFreeRingBuffer<float> *ringBuffer);

        /**
         * @brief Destructor.
         */
        ~AudioProcessingLayer();

        AudioProcessingLayer(const AudioProcessingLayer &) = delete;

        AudioProcessingLayer &operator=(const AudioProcessingLayer &) = delete;

        AudioProcessingLayer(AudioProcessingLayer &&) = delete;

        AudioProcessingLayer &operator=(AudioProcessingLayer &&) = delete;

        /**
         * @brief Initializes the audio device with specific parameters.
         * @param deviceId ID of the audio device to use.
         * @param sampleRate Desired sample rate in Hz.
         * @param bufferSizeFrames Desired buffer size in frames.
         * @return True if initialization was successful, false otherwise.
         */
        bool Initialize(uint32_t deviceId, float sampleRate, uint32_t bufferSizeFrames);

        /**
         * @brief Initializes the default audio device.
         * @param sampleRate Desired sample rate in Hz.
         * @param bufferSizeFrames Desired buffer size in frames.
         * @return True if initialization was successful, false otherwise.
         */
        bool InitializeDefault(float sampleRate, uint32_t bufferSizeFrames);

        /**
         * @brief Starts the audio stream.
         * @return True if started successfully, false otherwise.
         */
        bool Start();

        /**
         * @brief Stops the audio stream.
         */
        void Stop();

        /**
         * @brief Shuts down the audio layer and releases resources.
         */
        void Shutdown();

        /**
         * @brief Checks if the audio device is open.
         * @return True if open, false otherwise.
         */
        bool IsOpen() const;

        /**
         * @brief Checks if the audio stream is running.
         * @return True if running, false otherwise.
         */
        bool IsRunning() const;

    private:
        /**
         * @brief Static audio callback function used by the audio device.
         * @param inputBuffer Buffer containing input audio samples.
         * @param outputBuffer Buffer for output audio samples (unused here).
         * @param userData User data pointer (AudioProcessingLayer instance).
         * @return 0 on success, non-zero on error.
         */
        static int AudioCallback(std::span<const float> inputBuffer, std::span<float> outputBuffer, void *userData);

        /**
         * @brief Processes incoming audio data.
         * @param inputBuffer Buffer containing input audio samples.
         */
        void OnAudioInput(std::span<const float> inputBuffer);

        Util::LockFreeRingBuffer<float> *ringBuffer; ///< Pointer to the ring buffer for thread-safe data transfer.
        std::unique_ptr<GuitarIO::RtAudioDevice> audioDevice; ///< Audio device instance.
        uint32_t bufferSize;                                  ///< Current buffer size in frames.
    };

} // namespace GuitarDiagnostics::App
