#include "App/AudioProcessingLayer.h"

#include <AudioDevice.h>

#include <cmath>
#include <span>

namespace GuitarDiagnostics::App
{

    AudioProcessingLayer::AudioProcessingLayer(Util::LockFreeRingBuffer<float> *ringBuffer)
        : ringBuffer(ringBuffer), audioDevice(nullptr), bufferSize(0), peakInputLevel(0.0f)
    {
    }

    AudioProcessingLayer::~AudioProcessingLayer()
    {
        Shutdown();
    }

    bool AudioProcessingLayer::Initialize(uint32_t deviceId, float sampleRate, uint32_t bufferSizeFrames)
    {
        if (audioDevice && audioDevice->IsOpen())
        {
            return false;
        }

        this->bufferSize = bufferSizeFrames;
        audioDevice = std::make_unique<GuitarIO::RtAudioDevice>();

        GuitarIO::AudioStreamConfig config;
        config.sampleRate = static_cast<uint32_t>(sampleRate);
        config.bufferSize = bufferSizeFrames;
        config.inputChannels = 1;
        config.outputChannels = 0;

        GuitarIO::AudioCallback callback =
            [](std::span<const float> inputBuffer, std::span<float> outputBuffer, void *userData) -> int {
            return AudioProcessingLayer::AudioCallback(inputBuffer, outputBuffer, userData);
        };

        return audioDevice->Open(deviceId, config, callback, this);
    }

    bool AudioProcessingLayer::InitializeDefault(float sampleRate, uint32_t bufferSizeFrames)
    {
        if (audioDevice && audioDevice->IsOpen())
        {
            return false;
        }

        this->bufferSize = bufferSizeFrames;
        audioDevice = std::make_unique<GuitarIO::RtAudioDevice>();

        GuitarIO::AudioStreamConfig config;
        config.sampleRate = static_cast<uint32_t>(sampleRate);
        config.bufferSize = bufferSizeFrames;
        config.inputChannels = 1;
        config.outputChannels = 0;

        GuitarIO::AudioCallback callback =
            [](std::span<const float> inputBuffer, std::span<float> outputBuffer, void *userData) -> int {
            return AudioProcessingLayer::AudioCallback(inputBuffer, outputBuffer, userData);
        };

        return audioDevice->OpenDefault(config, callback, this);
    }

    bool AudioProcessingLayer::Start()
    {
        if (!audioDevice || !audioDevice->IsOpen())
        {
            return false;
        }

        return audioDevice->Start();
    }

    void AudioProcessingLayer::Stop()
    {
        if (audioDevice && audioDevice->IsRunning())
        {
            audioDevice->Stop();
        }
    }

    void AudioProcessingLayer::Shutdown()
    {
        Stop();

        if (audioDevice && audioDevice->IsOpen())
        {
            audioDevice->Close();
        }

        audioDevice.reset();
    }

    bool AudioProcessingLayer::IsOpen() const
    {
        return audioDevice && audioDevice->IsOpen();
    }

    bool AudioProcessingLayer::IsRunning() const
    {
        return audioDevice && audioDevice->IsRunning();
    }

    float AudioProcessingLayer::GetPeakInputLevel() const
    {
        return peakInputLevel.load(std::memory_order_relaxed);
    }

    int AudioProcessingLayer::AudioCallback(std::span<const float> inputBuffer,
        [[maybe_unused]] std::span<float> outputBuffer,
        void *userData)
    {
        auto *layer = static_cast<AudioProcessingLayer *>(userData);
        if (layer)
        {
            layer->OnAudioInput(inputBuffer);
        }
        return 0;
    }

    void AudioProcessingLayer::OnAudioInput(std::span<const float> inputBuffer)
    {
        if (!inputBuffer.empty() && ringBuffer)
        {
            ringBuffer->Write(inputBuffer);

            // Calculate RMS for peak level metering (real-time safe)
            float sumSquares = 0.0f;
            for (const float sample : inputBuffer)
            {
                sumSquares += sample * sample;
            }
            float rms = std::sqrt(sumSquares / static_cast<float>(inputBuffer.size()));

            // Store peak level atomically for UI thread to read
            peakInputLevel.store(rms, std::memory_order_relaxed);
        }
    }

} // namespace GuitarDiagnostics::App
