// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "soundDeviceData.h"

#include <gtest/gtest.h>

#include <QString>
#include <climits>

// SoundDeviceData is a plain data class (no Q_OBJECT, no signals).
//
// NOTE: port()/setPort() are declared in soundDeviceData.h but NOT implemented
// in soundDeviceData.cpp (and there is no backing m_port member). Calling them
// would cause a link error, so they are intentionally not tested here.
// See the delivery comment for the defect report.
//
// NOTE: cardId is not initialized in the constructor, so its post-construction
// value is indeterminate. We only exercise setCardId/getCardId round-trips and
// do not assert a constructor default for cardId. See the delivery comment.

// ---- Constructor defaults ----

TEST(SoundDeviceData, ConstructorDefaults)
{
    SoundDeviceData data;
    EXPECT_FALSE(data.ischecked());
    EXPECT_EQ(data.name(), QStringLiteral(""));
    EXPECT_EQ(data.getPortId(), QStringLiteral(""));
}

// ---- ischecked / setIschecked ----

TEST(SoundDeviceData, SetIscheckedTrue)
{
    SoundDeviceData data;
    data.setIschecked(true);
    EXPECT_TRUE(data.ischecked());
}

TEST(SoundDeviceData, SetIscheckedFalse)
{
    SoundDeviceData data;
    data.setIschecked(true);
    data.setIschecked(false);
    EXPECT_FALSE(data.ischecked());
}

TEST(SoundDeviceData, SetIscheckedToggle)
{
    SoundDeviceData data;
    data.setIschecked(true);
    EXPECT_TRUE(data.ischecked());
    data.setIschecked(false);
    EXPECT_FALSE(data.ischecked());
    data.setIschecked(true);
    EXPECT_TRUE(data.ischecked());
}

// ---- name / setName ----

TEST(SoundDeviceData, SetNameSimple)
{
    SoundDeviceData data;
    data.setName(QStringLiteral("Headphone"));
    EXPECT_EQ(data.name(), QStringLiteral("Headphone"));
}

TEST(SoundDeviceData, SetNameOverwrites)
{
    SoundDeviceData data;
    data.setName(QStringLiteral("first"));
    data.setName(QStringLiteral("second"));
    EXPECT_EQ(data.name(), QStringLiteral("second"));
}

TEST(SoundDeviceData, SetNameEmpty)
{
    SoundDeviceData data;
    data.setName(QStringLiteral("x"));
    data.setName(QStringLiteral(""));
    EXPECT_EQ(data.name(), QStringLiteral(""));
}

TEST(SoundDeviceData, SetNameUnicode)
{
    SoundDeviceData data;
    data.setName(QStringLiteral("耳机输出"));
    EXPECT_EQ(data.name(), QStringLiteral("耳机输出"));
}

TEST(SoundDeviceData, SetNameSpecialChars)
{
    SoundDeviceData data;
    const QString value = QStringLiteral("a/b\\c:d;e f");
    data.setName(value);
    EXPECT_EQ(data.name(), value);
}

// ---- getPortId / setPortId ----

TEST(SoundDeviceData, SetPortIdSimple)
{
    SoundDeviceData data;
    data.setPortId(QStringLiteral("port-1"));
    EXPECT_EQ(data.getPortId(), QStringLiteral("port-1"));
}

TEST(SoundDeviceData, SetPortIdOverwrites)
{
    SoundDeviceData data;
    data.setPortId(QStringLiteral("first"));
    data.setPortId(QStringLiteral("second"));
    EXPECT_EQ(data.getPortId(), QStringLiteral("second"));
}

TEST(SoundDeviceData, SetPortIdEmpty)
{
    SoundDeviceData data;
    data.setPortId(QStringLiteral("x"));
    data.setPortId(QStringLiteral(""));
    EXPECT_EQ(data.getPortId(), QStringLiteral(""));
}

// ---- getCardId / setCardId ----

TEST(SoundDeviceData, SetCardIdSimple)
{
    SoundDeviceData data;
    data.setCardId(7);
    EXPECT_EQ(data.getCardId(), 7u);
}

TEST(SoundDeviceData, SetCardIdOverwrites)
{
    SoundDeviceData data;
    data.setCardId(1);
    data.setCardId(2);
    EXPECT_EQ(data.getCardId(), 2u);
}

TEST(SoundDeviceData, SetCardIdZero)
{
    SoundDeviceData data;
    data.setCardId(99);
    data.setCardId(0);
    EXPECT_EQ(data.getCardId(), 0u);
}

TEST(SoundDeviceData, SetCardIdMaxValue)
{
    SoundDeviceData data;
    data.setCardId(UINT_MAX);
    EXPECT_EQ(data.getCardId(), UINT_MAX);
}
