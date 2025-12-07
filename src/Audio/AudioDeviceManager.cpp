#include "Audio/AudioDeviceManager.h"

namespace GuitarDiagnostics::Audio
{

    AudioDeviceInfo::AudioDeviceInfo()
        : name(), id(0), maxInputChannels(0), maxOutputChannels(0), supportedSampleRates()
    {
    }

    AudioDeviceManager::AudioDeviceManager() : guitarIOManager(GuitarIO::AudioDeviceManager::Get())
    {
    }

    AudioDeviceManager::~AudioDeviceManager()
    {
    }

    std::vector<AudioDeviceInfo> AudioDeviceManager::EnumerateInputDevices() const
    {
        auto guitarIODevices = guitarIOManager.EnumerateInputDevices();
        std::vector<AudioDeviceInfo> devices;
        devices.reserve(guitarIODevices.size());

        for (const auto &guitarIODevice : guitarIODevices)
        {
            AudioDeviceInfo info;
            info.name = guitarIODevice.name;
            info.id = guitarIODevice.id;
            info.maxInputChannels = guitarIODevice.maxInputChannels;
            info.maxOutputChannels = guitarIODevice.maxOutputChannels;
            info.supportedSampleRates = guitarIODevice.supportedSampleRates;
            devices.push_back(info);
        }

        return devices;
    }

    std::vector<AudioDeviceInfo> AudioDeviceManager::EnumerateOutputDevices() const
    {
        auto guitarIODevices = guitarIOManager.EnumerateOutputDevices();
        std::vector<AudioDeviceInfo> devices;
        devices.reserve(guitarIODevices.size());

        for (const auto &guitarIODevice : guitarIODevices)
        {
            AudioDeviceInfo info;
            info.name = guitarIODevice.name;
            info.id = guitarIODevice.id;
            info.maxInputChannels = guitarIODevice.maxInputChannels;
            info.maxOutputChannels = guitarIODevice.maxOutputChannels;
            info.supportedSampleRates = guitarIODevice.supportedSampleRates;
            devices.push_back(info);
        }

        return devices;
    }

    uint32_t AudioDeviceManager::GetDefaultInputDevice() const
    {
        return guitarIOManager.GetDefaultInputDevice();
    }

    uint32_t AudioDeviceManager::GetDefaultOutputDevice() const
    {
        return guitarIOManager.GetDefaultOutputDevice();
    }

    AudioDeviceInfo AudioDeviceManager::GetDeviceInfo(uint32_t deviceId) const
    {
        auto guitarIOInfo = guitarIOManager.GetDeviceInfo(deviceId);

        AudioDeviceInfo info;
        info.name = guitarIOInfo.name;
        info.id = guitarIOInfo.id;
        info.maxInputChannels = guitarIOInfo.maxInputChannels;
        info.maxOutputChannels = guitarIOInfo.maxOutputChannels;
        info.supportedSampleRates = guitarIOInfo.supportedSampleRates;

        return info;
    }

} // namespace GuitarDiagnostics::Audio
