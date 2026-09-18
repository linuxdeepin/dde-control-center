// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bluetoothdevice.h"

#include <gtest/gtest.h>

#include <QSignalSpy>
#include <QString>
#include <QDebug>

// ---- Constructor defaults ----

TEST(BluetoothDevice, ConstructorDefaults)
{
    BluetoothDevice dev;
    EXPECT_EQ(dev.id(), QStringLiteral(""));
    EXPECT_EQ(dev.name(), QStringLiteral(""));
    EXPECT_EQ(dev.alias(), QStringLiteral(""));
    EXPECT_EQ(dev.address(), QStringLiteral(""));
    EXPECT_FALSE(dev.paired());
    EXPECT_FALSE(dev.trusted());
    EXPECT_FALSE(dev.connecting());
    EXPECT_FALSE(dev.connectState());
    EXPECT_EQ(dev.state(), BluetoothDevice::StateUnavailable);
    EXPECT_EQ(dev.battery(), 0);
    EXPECT_EQ(dev.deviceType(), QStringLiteral(""));
    EXPECT_FALSE(dev.canSendFile());
}

// ---- setId / setAddress (no signal) ----

TEST(BluetoothDevice, SetIdNoSignal)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::nameChanged);  // any unrelated spy

    dev.setId(QStringLiteral("dev1"));
    EXPECT_EQ(dev.id(), QStringLiteral("dev1"));
    EXPECT_EQ(spy.count(), 0);  // setId has no signal at all
}

TEST(BluetoothDevice, SetIdOverwrites)
{
    BluetoothDevice dev;
    dev.setId("first");
    dev.setId("second");
    EXPECT_EQ(dev.id(), QStringLiteral("second"));
}

TEST(BluetoothDevice, SetAddressNoSignal)
{
    BluetoothDevice dev;
    dev.setAddress(QStringLiteral("AA:BB:CC"));
    EXPECT_EQ(dev.address(), QStringLiteral("AA:BB:CC"));
}

TEST(BluetoothDevice, SetAddressOverwrites)
{
    BluetoothDevice dev;
    dev.setAddress("first");
    dev.setAddress("second");
    EXPECT_EQ(dev.address(), QStringLiteral("second"));
}

// ---- setName (signal on change) ----

TEST(BluetoothDevice, SetNameEmitsOnChange)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::nameChanged);

    dev.setName(QStringLiteral("MyDevice"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("MyDevice"));
    EXPECT_EQ(dev.name(), QStringLiteral("MyDevice"));
}

TEST(BluetoothDevice, SetNameNoSignalOnSameValue)
{
    BluetoothDevice dev;
    dev.setName(QStringLiteral("MyDevice"));

    QSignalSpy spy(&dev, &BluetoothDevice::nameChanged);
    dev.setName(QStringLiteral("MyDevice"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- setAlias ----

TEST(BluetoothDevice, SetAliasEmitsOnChange)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::aliasChanged);

    dev.setAlias(QStringLiteral("Alias1"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("Alias1"));
    EXPECT_EQ(dev.alias(), QStringLiteral("Alias1"));
}

TEST(BluetoothDevice, SetAliasNoSignalOnSameValue)
{
    BluetoothDevice dev;
    dev.setAlias(QStringLiteral("Alias1"));

    QSignalSpy spy(&dev, &BluetoothDevice::aliasChanged);
    dev.setAlias(QStringLiteral("Alias1"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- setPaired ----

TEST(BluetoothDevice, SetPairedEmitsOnChange)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::pairedChanged);

    dev.setPaired(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(spy.takeFirst().at(0).toBool());
    EXPECT_TRUE(dev.paired());
}

TEST(BluetoothDevice, SetPairedNoSignalOnSameValue)
{
    BluetoothDevice dev;
    dev.setPaired(true);

    QSignalSpy spy(&dev, &BluetoothDevice::pairedChanged);
    dev.setPaired(true);
    EXPECT_EQ(spy.count(), 0);
}

TEST(BluetoothDevice, SetPairedFalseEmits)
{
    BluetoothDevice dev;
    dev.setPaired(true);

    QSignalSpy spy(&dev, &BluetoothDevice::pairedChanged);
    dev.setPaired(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(dev.paired());
}

// ---- setState (emits if state OR connectState changed) ----

TEST(BluetoothDevice, SetStateEmitsWhenStateChanges)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::stateChanged);

    dev.setState(BluetoothDevice::StateAvailable, false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(dev.state(), BluetoothDevice::StateAvailable);
}

TEST(BluetoothDevice, SetStateEmitsWhenConnectStateChanges)
{
    BluetoothDevice dev;
    dev.setState(BluetoothDevice::StateConnected, false);

    QSignalSpy spy(&dev, &BluetoothDevice::stateChanged);
    // Same state, different connectState → emits.
    dev.setState(BluetoothDevice::StateConnected, true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(dev.connectState());
}

TEST(BluetoothDevice, SetStateNoSignalWhenBothUnchanged)
{
    BluetoothDevice dev;
    dev.setState(BluetoothDevice::StateConnected, true);

    QSignalSpy spy(&dev, &BluetoothDevice::stateChanged);
    dev.setState(BluetoothDevice::StateConnected, true);
    EXPECT_EQ(spy.count(), 0);
}

TEST(BluetoothDevice, SetStateAllValues)
{
    BluetoothDevice dev;
    dev.setState(BluetoothDevice::StateUnavailable, false);
    EXPECT_EQ(dev.state(), BluetoothDevice::StateUnavailable);

    dev.setState(BluetoothDevice::StateAvailable, false);
    EXPECT_EQ(dev.state(), BluetoothDevice::StateAvailable);

    dev.setState(BluetoothDevice::StateConnected, true);
    EXPECT_EQ(dev.state(), BluetoothDevice::StateConnected);
    EXPECT_TRUE(dev.connectState());

    dev.setState(BluetoothDevice::StateDisconnecting, true);
    EXPECT_EQ(dev.state(), BluetoothDevice::StateDisconnecting);
}

// ---- setTrusted ----

TEST(BluetoothDevice, SetTrustedEmitsOnChange)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::trustedChanged);

    dev.setTrusted(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(spy.takeFirst().at(0).toBool());
    EXPECT_TRUE(dev.trusted());
}

TEST(BluetoothDevice, SetTrustedNoSignalOnSameValue)
{
    BluetoothDevice dev;
    dev.setTrusted(true);

    QSignalSpy spy(&dev, &BluetoothDevice::trustedChanged);
    dev.setTrusted(true);
    EXPECT_EQ(spy.count(), 0);
}

// ---- setConnecting ----

TEST(BluetoothDevice, SetConnectingEmitsOnChange)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::connectingChanged);

    dev.setConnecting(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(spy.takeFirst().at(0).toBool());
    EXPECT_TRUE(dev.connecting());
}

TEST(BluetoothDevice, SetConnectingNoSignalOnSameValue)
{
    BluetoothDevice dev;
    dev.setConnecting(true);

    QSignalSpy spy(&dev, &BluetoothDevice::connectingChanged);
    dev.setConnecting(true);
    EXPECT_EQ(spy.count(), 0);
}

// ---- setBattery ----

TEST(BluetoothDevice, SetBatteryEmitsOnChange)
{
    BluetoothDevice dev;
    QSignalSpy spy(&dev, &BluetoothDevice::batteryChanged);

    dev.setBattery(80);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toInt(), 80);
    EXPECT_EQ(dev.battery(), 80);
}

TEST(BluetoothDevice, SetBatteryNoSignalOnSameValue)
{
    BluetoothDevice dev;
    dev.setBattery(50);

    QSignalSpy spy(&dev, &BluetoothDevice::batteryChanged);
    dev.setBattery(50);
    EXPECT_EQ(spy.count(), 0);
}

TEST(BluetoothDevice, SetBatteryZero)
{
    BluetoothDevice dev;
    dev.setBattery(30);

    QSignalSpy spy(&dev, &BluetoothDevice::batteryChanged);
    dev.setBattery(0);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(dev.battery(), 0);
}

TEST(BluetoothDevice, SetBatteryFullRange)
{
    BluetoothDevice dev;
    dev.setBattery(0);
    EXPECT_EQ(dev.battery(), 0);

    dev.setBattery(100);
    EXPECT_EQ(dev.battery(), 100);
}

// ---- setDeviceType (maps via deviceType2Icon, fallback bluetooth_other) ----

TEST(BluetoothDevice, SetDeviceTypeComputerMapsToPc)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("computer"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_pc"));
    EXPECT_TRUE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypePhoneMapsToPhone)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("phone"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_phone"));
    EXPECT_TRUE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeUnknownFallsBackToOther)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("nonexistent_type"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_other"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeKeyboard)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("input-keyboard"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_keyboard"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeMouse)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("input-mouse"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_mouse"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeVideoDisplay)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("video-display"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_vidicon"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeAudioCard)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("audio-card"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_pheadset"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypePrinter)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("printer"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_print"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeCamera)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("camera-photo"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_camera"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeNetworkWireless)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("network-wireless"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_lan"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeScanner)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("scanner"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_scaner"));
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, SetDeviceTypeOverwrites)
{
    BluetoothDevice dev;
    dev.setDeviceType(QStringLiteral("computer"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_pc"));

    dev.setDeviceType(QStringLiteral("phone"));
    EXPECT_EQ(dev.deviceType(), QStringLiteral("bluetooth_phone"));
}

// ---- deviceType2Icon full mapping coverage (parameterized) ----

namespace {
struct DeviceTypeMapping {
    const char *input;
    const char *expectedIcon;
    bool canSend;
};
} // namespace

class BluetoothDeviceTypeMapping
    : public testing::TestWithParam<DeviceTypeMapping>
{
};

TEST_P(BluetoothDeviceTypeMapping, IconAndCanSendFileMatch)
{
    const auto &param = GetParam();
    BluetoothDevice dev;
    dev.setDeviceType(QString::fromLatin1(param.input));
    EXPECT_EQ(dev.deviceType(), QString::fromLatin1(param.expectedIcon));
    EXPECT_EQ(dev.canSendFile(), param.canSend);
}

INSTANTIATE_TEST_SUITE_P(AllDeviceType2IconMappings, BluetoothDeviceTypeMapping,
    testing::Values(
        DeviceTypeMapping{"unknow", "bluetooth_other", false},
        DeviceTypeMapping{"computer", "bluetooth_pc", true},
        DeviceTypeMapping{"phone", "bluetooth_phone", true},
        DeviceTypeMapping{"video-display", "bluetooth_vidicon", false},
        DeviceTypeMapping{"multimedia-player", "bluetooth_tv", false},
        DeviceTypeMapping{"scanner", "bluetooth_scaner", false},
        DeviceTypeMapping{"input-keyboard", "bluetooth_keyboard", false},
        DeviceTypeMapping{"input-mouse", "bluetooth_mouse", false},
        DeviceTypeMapping{"input-gaming", "bluetooth_other", false},
        DeviceTypeMapping{"input-tablet", "bluetooth_touchpad", false},
        DeviceTypeMapping{"audio-card", "bluetooth_pheadset", false},
        DeviceTypeMapping{"audio-headset", "bluetooth_pheadset", false},
        DeviceTypeMapping{"audio-headphones", "bluetooth_pheadset", false},
        DeviceTypeMapping{"network-wireless", "bluetooth_lan", false},
        DeviceTypeMapping{"camera-video", "bluetooth_vidicon", false},
        DeviceTypeMapping{"printer", "bluetooth_print", false},
        DeviceTypeMapping{"camera-photo", "bluetooth_camera", false},
        DeviceTypeMapping{"modem", "bluetooth_other", false}
    ));

// ---- canSendFile ----

TEST(BluetoothDevice, CanSendFileFalseByDefault)
{
    BluetoothDevice dev;
    EXPECT_FALSE(dev.canSendFile());
}

TEST(BluetoothDevice, CanSendFileTrueOnlyForPcAndPhone)
{
    BluetoothDevice dev;
    dev.setDeviceType("computer");
    EXPECT_TRUE(dev.canSendFile());

    BluetoothDevice dev2;
    dev2.setDeviceType("phone");
    EXPECT_TRUE(dev2.canSendFile());
}

// ---- operator<< ----

TEST(BluetoothDevice, DebugStreamOperator)
{
    BluetoothDevice dev;
    dev.setName("TestDev");
    dev.setPaired(true);
    dev.setState(BluetoothDevice::StateConnected, true);

    QString output;
    {
        QDebug dbg(&output);
        dbg << &dev;
    }
    EXPECT_TRUE(output.contains(QStringLiteral("BluetoothDevice name:")));
    EXPECT_TRUE(output.contains(QStringLiteral("TestDev")));
}
