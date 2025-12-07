#pragma once

#include "Audio/AudioDeviceManager.h"
#include "Util/LockFreeRingBuffer.h"

#include <RtAudioDevice.h>

#include <cstdint>
#include <memory>

namespace GuitarDiagnostics::App
{

    class AudioProcessingLayer
    {
    public:
        AudioProcessingLayer(Util::LockFreeRingBuffer<float> *ringBuffer);
        ~AudioProcessingLayer();

        AudioProcessingLayer(const AudioProcessingLayer &) = delete;
        AudioProcessingLayer &operator=(const AudioProcessingLayer &) = delete;
        AudioProcessingLayer(AudioProcessingLayer &&) = delete;
        AudioProcessingLayer &operator=(AudioProcessingLayer &&) = delete;

        bool Initialize(uint32_t deviceId, float sampleRate, uint32_t bufferSizeFrames);
        bool InitializeDefault(float sampleRate, uint32_t bufferSizeFrames);
        bool Start();
        void Stop();
        void Shutdown();

        bool IsOpen() const;
        bool IsRunning() const;

    private:
        static int AudioCallback(std::span<const float> inputBuffer, std::span<float> outputBuffer, void *userData);

        void OnAudioInput(std::span<const float> inputBuffer);

        Util::LockFreeRingBuffer<float> *ringBuffer;
        std::unique_ptr<GuitarIO::RtAudioDevice> audioDevice;
        uint32_t bufferSize;
    };

} // namespace GuitarDiagnostics::App
