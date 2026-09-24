// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "port.h"

#include <gtest/gtest.h>

#include <QSignalSpy>
#include <climits>
#include <QString>

// ---- Constructor defaults ----

TEST(Port, ConstructorDefaults)
{
    Port port(nullptr);
    EXPECT_EQ(port.id(), QStringLiteral(""));
    EXPECT_EQ(port.name(), QStringLiteral(""));
    EXPECT_EQ(port.cardName(), QStringLiteral(""));
    EXPECT_EQ(port.cardId(), 0u);
    EXPECT_FALSE(port.isActive());
    EXPECT_FALSE(port.isEnabled());
    EXPECT_FALSE(port.isBluetoothPort());
    EXPECT_EQ(port.direction(), Port::Out);
}

TEST(Port, DirectionEnumValues)
{
    EXPECT_EQ(static_cast<int>(Port::Out), 1);
    EXPECT_EQ(static_cast<int>(Port::In), 2);
}

// ---- setId ----

TEST(Port, SetIdEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::idChanged);

    port.setId(QStringLiteral("hdmi"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("hdmi"));
    EXPECT_EQ(port.id(), QStringLiteral("hdmi"));
}

TEST(Port, SetIdNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setId(QStringLiteral("hdmi"));

    QSignalSpy spy(&port, &Port::idChanged);
    port.setId(QStringLiteral("hdmi"));
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetIdOverwrites)
{
    Port port(nullptr);
    port.setId("first");
    port.setId("second");
    EXPECT_EQ(port.id(), QStringLiteral("second"));
}

TEST(Port, SetIdFromEmptyToNonEmpty)
{
    Port port(nullptr);
    port.setId(QStringLiteral("abc"));
    EXPECT_EQ(port.id(), QStringLiteral("abc"));
}

TEST(Port, SetIdFromNonEmptyToEmpty)
{
    Port port(nullptr);
    port.setId(QStringLiteral("abc"));
    port.setId(QStringLiteral(""));
    EXPECT_EQ(port.id(), QStringLiteral(""));
}

// ---- setName ----

TEST(Port, SetNameEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::nameChanged);

    port.setName(QStringLiteral("Speaker"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("Speaker"));
    EXPECT_EQ(port.name(), QStringLiteral("Speaker"));
}

TEST(Port, SetNameNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setName(QStringLiteral("Speaker"));

    QSignalSpy spy(&port, &Port::nameChanged);
    port.setName(QStringLiteral("Speaker"));
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetNameOverwrites)
{
    Port port(nullptr);
    port.setName("first");
    port.setName("second");
    EXPECT_EQ(port.name(), QStringLiteral("second"));
}

// ---- setCardName ----

TEST(Port, SetCardNameEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::cardNameChanged);

    port.setCardName(QStringLiteral("HDA"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toString(), QStringLiteral("HDA"));
    EXPECT_EQ(port.cardName(), QStringLiteral("HDA"));
}

TEST(Port, SetCardNameNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setCardName(QStringLiteral("HDA"));

    QSignalSpy spy(&port, &Port::cardNameChanged);
    port.setCardName(QStringLiteral("HDA"));
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetCardNameOverwrites)
{
    Port port(nullptr);
    port.setCardName("first");
    port.setCardName("second");
    EXPECT_EQ(port.cardName(), QStringLiteral("second"));
}

// ---- setIsActive (direction-dependent signal) ----

TEST(Port, SetIsActiveEmitsOutputActiveWhenDirectionOut)
{
    Port port(nullptr);
    ASSERT_EQ(port.direction(), Port::Out);

    QSignalSpy outSpy(&port, &Port::isOutputActiveChanged);
    QSignalSpy inSpy(&port, &Port::isInputActiveChanged);

    port.setIsActive(true);
    ASSERT_EQ(outSpy.count(), 1);
    EXPECT_TRUE(outSpy.takeFirst().at(0).toBool());
    EXPECT_EQ(inSpy.count(), 0);
    EXPECT_TRUE(port.isActive());
}

TEST(Port, SetIsActiveEmitsInputActiveWhenDirectionIn)
{
    Port port(nullptr);
    port.setDirection(Port::In);
    ASSERT_EQ(port.direction(), Port::In);

    QSignalSpy outSpy(&port, &Port::isOutputActiveChanged);
    QSignalSpy inSpy(&port, &Port::isInputActiveChanged);

    port.setIsActive(true);
    ASSERT_EQ(inSpy.count(), 1);
    EXPECT_TRUE(inSpy.takeFirst().at(0).toBool());
    EXPECT_EQ(outSpy.count(), 0);
    EXPECT_TRUE(port.isActive());
}

TEST(Port, SetIsActiveNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setIsActive(true);

    QSignalSpy spy(&port, &Port::isOutputActiveChanged);
    port.setIsActive(true);
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetIsActiveFalseAfterTrueEmits)
{
    Port port(nullptr);
    port.setIsActive(true);

    QSignalSpy spy(&port, &Port::isOutputActiveChanged);
    port.setIsActive(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(port.isActive());
}

TEST(Port, SetIsActiveDirectionSwitchChangesEmittedSignal)
{
    Port port(nullptr);

    // Default Out → toggling emits isOutputActiveChanged
    QSignalSpy outSpy(&port, &Port::isOutputActiveChanged);
    port.setIsActive(true);
    EXPECT_EQ(outSpy.count(), 1);
    outSpy.clear();

    // Switch to In → toggling emits isInputActiveChanged, not output
    QSignalSpy inSpy(&port, &Port::isInputActiveChanged);
    port.setDirection(Port::In);
    outSpy.clear();

    port.setIsActive(false);
    EXPECT_EQ(inSpy.count(), 1);
    EXPECT_EQ(outSpy.count(), 0);
}

// ---- setDirection ----
// Direction enum is not declared with Q_ENUM, so QSignalSpy cannot reliably
// decode the argument metatype. We verify via spy.count() + the getter directly,
// mirroring the BluetoothDevice::stateChanged precedent in ut_bluetoothdevice.cpp.

TEST(Port, SetDirectionEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::directionChanged);

    port.setDirection(Port::In);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(port.direction(), Port::In);
}

TEST(Port, SetDirectionNoSignalOnSameValue)
{
    Port port(nullptr);
    ASSERT_EQ(port.direction(), Port::Out);

    QSignalSpy spy(&port, &Port::directionChanged);
    port.setDirection(Port::Out);
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetDirectionToggleBackAndForth)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::directionChanged);

    port.setDirection(Port::In);
    port.setDirection(Port::Out);
    port.setDirection(Port::In);
    EXPECT_EQ(spy.count(), 3);
    EXPECT_EQ(port.direction(), Port::In);
}

// ---- setCardId ----

TEST(Port, SetCardIdEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::cardIdChanged);

    port.setCardId(5u);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.takeFirst().at(0).toUInt(), 5u);
    EXPECT_EQ(port.cardId(), 5u);
}

TEST(Port, SetCardIdNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setCardId(5u);

    QSignalSpy spy(&port, &Port::cardIdChanged);
    port.setCardId(5u);
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetCardIdZero)
{
    Port port(nullptr);
    port.setCardId(10u);
    port.setCardId(0u);
    EXPECT_EQ(port.cardId(), 0u);
}

TEST(Port, SetCardIdLargeValue)
{
    Port port(nullptr);
    port.setCardId(UINT_MAX);
    EXPECT_EQ(port.cardId(), UINT_MAX);
}

// ---- setEnabled ----

TEST(Port, SetEnabledEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::currentPortEnabled);

    port.setEnabled(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(spy.takeFirst().at(0).toBool());
    EXPECT_TRUE(port.isEnabled());
}

TEST(Port, SetEnabledNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setEnabled(true);

    QSignalSpy spy(&port, &Port::currentPortEnabled);
    port.setEnabled(true);
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetEnabledFalseAfterTrue)
{
    Port port(nullptr);
    port.setEnabled(true);

    QSignalSpy spy(&port, &Port::currentPortEnabled);
    port.setEnabled(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(port.isEnabled());
}

// ---- setIsBluetoothPort ----

TEST(Port, SetIsBluetoothPortEmitsOnChange)
{
    Port port(nullptr);
    QSignalSpy spy(&port, &Port::currentBluetoothPortChanged);

    port.setIsBluetoothPort(true);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_TRUE(spy.takeFirst().at(0).toBool());
    EXPECT_TRUE(port.isBluetoothPort());
}

TEST(Port, SetIsBluetoothPortNoSignalOnSameValue)
{
    Port port(nullptr);
    port.setIsBluetoothPort(true);

    QSignalSpy spy(&port, &Port::currentBluetoothPortChanged);
    port.setIsBluetoothPort(true);
    EXPECT_EQ(spy.count(), 0);
}

TEST(Port, SetIsBluetoothPortFalseAfterTrue)
{
    Port port(nullptr);
    port.setIsBluetoothPort(true);

    QSignalSpy spy(&port, &Port::currentBluetoothPortChanged);
    port.setIsBluetoothPort(false);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toBool());
    EXPECT_FALSE(port.isBluetoothPort());
}
