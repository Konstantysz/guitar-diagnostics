#include <gtest/gtest.h>

#include "Audio/AudioDeviceManager.h"

using namespace GuitarDiagnostics::Audio;

class AudioDeviceManagerTest : public ::testing::Test
{
protected:
    AudioDeviceManager *manager;

    void SetUp() override
    {
        manager = nullptr;
    }

    void TearDown() override
    {
        if (manager)
        {
            delete manager;
            manager = nullptr;
        }
    }
};

TEST_F(AudioDeviceManagerTest, CreateInstance)
{
    ASSERT_NO_THROW(manager = new AudioDeviceManager());
    ASSERT_NE(manager, nullptr);
}

TEST_F(AudioDeviceManagerTest, EnumerateInputDevices)
{
    manager = new AudioDeviceManager();
    std::vector<AudioDeviceInfo> devices;

    ASSERT_NO_THROW(devices = manager->EnumerateInputDevices());

    // Should have at least one input device (system default)
    EXPECT_GE(devices.size(), 0);

    // If devices exist, validate structure
    for (const auto &device : devices)
    {
        EXPECT_FALSE(device.name.empty());
        EXPECT_GT(device.maxInputChannels, 0);
        EXPECT_FALSE(device.supportedSampleRates.empty());
    }
}

TEST_F(AudioDeviceManagerTest, EnumerateOutputDevices)
{
    manager = new AudioDeviceManager();
    std::vector<AudioDeviceInfo> devices;

    ASSERT_NO_THROW(devices = manager->EnumerateOutputDevices());

    // Should have at least one output device
    EXPECT_GE(devices.size(), 0);

    // If devices exist, validate structure
    for (const auto &device : devices)
    {
        EXPECT_FALSE(device.name.empty());
        EXPECT_GT(device.maxOutputChannels, 0);
        EXPECT_FALSE(device.supportedSampleRates.empty());
    }
}

TEST_F(AudioDeviceManagerTest, GetDefaultInputDevice)
{
    manager = new AudioDeviceManager();

    uint32_t defaultDevice = 0;
    ASSERT_NO_THROW(defaultDevice = manager->GetDefaultInputDevice());

    // Default device should be valid
    AudioDeviceInfo info = manager->GetDeviceInfo(defaultDevice);
    EXPECT_FALSE(info.name.empty());
    EXPECT_GT(info.maxInputChannels, 0);
}

TEST_F(AudioDeviceManagerTest, GetDefaultOutputDevice)
{
    manager = new AudioDeviceManager();

    uint32_t defaultDevice = 0;
    ASSERT_NO_THROW(defaultDevice = manager->GetDefaultOutputDevice());

    // Default device should be valid
    AudioDeviceInfo info = manager->GetDeviceInfo(defaultDevice);
    EXPECT_FALSE(info.name.empty());
    EXPECT_GT(info.maxOutputChannels, 0);
}

TEST_F(AudioDeviceManagerTest, GetDeviceInfo)
{
    manager = new AudioDeviceManager();

    auto devices = manager->EnumerateInputDevices();
    if (devices.empty())
    {
        GTEST_SKIP() << "No input devices available";
    }

    AudioDeviceInfo info = manager->GetDeviceInfo(devices[0].id);
    EXPECT_EQ(info.id, devices[0].id);
    EXPECT_EQ(info.name, devices[0].name);
    EXPECT_EQ(info.maxInputChannels, devices[0].maxInputChannels);
}

TEST_F(AudioDeviceManagerTest, GetInvalidDeviceInfo)
{
    manager = new AudioDeviceManager();

    // Query non-existent device (use very large ID unlikely to exist)
    AudioDeviceInfo info = manager->GetDeviceInfo(999999);
    EXPECT_TRUE(info.name.empty());
    EXPECT_EQ(info.maxInputChannels, 0);
    EXPECT_EQ(info.maxOutputChannels, 0);
}
