#pragma once

#include <AudioDeviceManager.h>

#include <cstdint>
#include <string>
#include <vector>

namespace GuitarDiagnostics::Audio
{

    struct AudioDeviceInfo
    {
        std::string name;
        uint32_t id;
        uint32_t maxInputChannels;
        uint32_t maxOutputChannels;
        std::vector<uint32_t> supportedSampleRates;

        AudioDeviceInfo();
    };

    class AudioDeviceManager
    {
    public:
        AudioDeviceManager();
        ~AudioDeviceManager();

        AudioDeviceManager(const AudioDeviceManager &) = delete;
        AudioDeviceManager &operator=(const AudioDeviceManager &) = delete;
        AudioDeviceManager(AudioDeviceManager &&) = delete;
        AudioDeviceManager &operator=(AudioDeviceManager &&) = delete;

        std::vector<AudioDeviceInfo> EnumerateInputDevices() const;
        std::vector<AudioDeviceInfo> EnumerateOutputDevices() const;
        uint32_t GetDefaultInputDevice() const;
        uint32_t GetDefaultOutputDevice() const;
        AudioDeviceInfo GetDeviceInfo(uint32_t deviceId) const;

    private:
        GuitarIO::AudioDeviceManager &guitarIOManager;
    };

} // namespace GuitarDiagnostics::Audio
