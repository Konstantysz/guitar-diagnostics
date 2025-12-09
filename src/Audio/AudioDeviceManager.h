#pragma once

#include <AudioDeviceManager.h>

#include <cstdint>
#include <string>
#include <vector>

namespace GuitarDiagnostics::Audio
{

    /**
     * @brief Information describing an available audio device.
     */
    struct AudioDeviceInfo
    {
        std::string name;                           ///< Device name.
        uint32_t id;                                ///< Device ID.
        uint32_t maxInputChannels;                  ///< Maximum input channels supported.
        uint32_t maxOutputChannels;                 ///< Maximum output channels supported.
        std::vector<uint32_t> supportedSampleRates; ///< List of supported sample rates.

        /**
         * @brief Constructs an empty AudioDeviceInfo.
         */
        AudioDeviceInfo();
    };

    /**
     * @brief Manages enumeration and retrieval of audio device information.
     *
     * Provides an interface to query available input and output devices.
     */
    class AudioDeviceManager
    {
    public:
        /**
         * @brief Constructs the AudioDeviceManager.
         */
        AudioDeviceManager();

        /**
         * @brief Destructor.
         */
        ~AudioDeviceManager();

        AudioDeviceManager(const AudioDeviceManager &) = delete;

        AudioDeviceManager &operator=(const AudioDeviceManager &) = delete;

        AudioDeviceManager(AudioDeviceManager &&) = delete;

        AudioDeviceManager &operator=(AudioDeviceManager &&) = delete;

        /**
         * @brief Enumerates available input devices.
         * @return Vector of AudioDeviceInfo for input devices.
         */
        std::vector<AudioDeviceInfo> EnumerateInputDevices() const;

        /**
         * @brief Enumerates available output devices.
         * @return Vector of AudioDeviceInfo for output devices.
         */
        std::vector<AudioDeviceInfo> EnumerateOutputDevices() const;

        /**
         * @brief Gets the ID of the default input device.
         * @return Default input device ID.
         */
        uint32_t GetDefaultInputDevice() const;

        /**
         * @brief Gets the ID of the default output device.
         * @return Default output device ID.
         */
        uint32_t GetDefaultOutputDevice() const;

        /**
         * @brief Retrieves detailed information for a specific device.
         * @param deviceId ID of the device to query.
         * @return AudioDeviceInfo struct containing device details.
         */
        AudioDeviceInfo GetDeviceInfo(uint32_t deviceId) const;

    private:
        GuitarIO::AudioDeviceManager &guitarIOManager; ///< Reference to the underlying GuitarIO manager.
    };

} // namespace GuitarDiagnostics::Audio
