// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gesturedata.h"

#include <gtest/gtest.h>

#include <QSignalSpy>
#include <QString>
#include <QList>

// ---- Constructor defaults ----

TEST(GestureData, ConstructorDefaults)
{
    GestureData gd;
    EXPECT_EQ(gd.actionType(), QStringLiteral(""));
    EXPECT_EQ(gd.gestureId(), QStringLiteral(""));
    EXPECT_EQ(gd.displayName(), QStringLiteral(""));
    EXPECT_EQ(gd.direction(), QStringLiteral(""));
    // m_fingersNum is not initialized in the constructor (gesturedata.h) — reading
    // it before setFingersNum is undefined behavior; skip the assertion, mirroring
    // ut_sounddevicedata's handling of the uninitialized cardId field.
    EXPECT_EQ(gd.actionName(), QStringLiteral(""));
    EXPECT_EQ(gd.sequence(), -1);
    EXPECT_TRUE(gd.actions().isEmpty());
}

// ---- actionType ----

TEST(GestureData, SetActionTypeEmitsOnChange)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::actionTypeChanged);

    gd.setActionType(QStringLiteral("tap"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.actionType(), QStringLiteral("tap"));
}

TEST(GestureData, SetActionTypeNoSignalOnSameValue)
{
    GestureData gd;
    gd.setActionType(QStringLiteral("tap"));

    QSignalSpy spy(&gd, &GestureData::actionTypeChanged);
    gd.setActionType(QStringLiteral("tap"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- gestureId ----

TEST(GestureData, SetGestureIdEmitsOnChange)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::gestureIdChanged);

    gd.setGestureId(QStringLiteral("swipe"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.gestureId(), QStringLiteral("swipe"));
}

TEST(GestureData, SetGestureIdNoSignalOnSameValue)
{
    GestureData gd;
    gd.setGestureId(QStringLiteral("swipe"));

    QSignalSpy spy(&gd, &GestureData::gestureIdChanged);
    gd.setGestureId(QStringLiteral("swipe"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- displayName ----

TEST(GestureData, SetDisplayNameEmitsOnChange)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::displayNameChanged);

    gd.setDisplayName(QStringLiteral("Tap"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.displayName(), QStringLiteral("Tap"));
}

TEST(GestureData, SetDisplayNameNoSignalOnSameValue)
{
    GestureData gd;
    gd.setDisplayName(QStringLiteral("Tap"));

    QSignalSpy spy(&gd, &GestureData::displayNameChanged);
    gd.setDisplayName(QStringLiteral("Tap"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- direction ----

TEST(GestureData, SetDirectionEmitsOnChange)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::directionChanged);

    gd.setDirection(QStringLiteral("left"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.direction(), QStringLiteral("left"));
}

TEST(GestureData, SetDirectionNoSignalOnSameValue)
{
    GestureData gd;
    gd.setDirection(QStringLiteral("left"));

    QSignalSpy spy(&gd, &GestureData::directionChanged);
    gd.setDirection(QStringLiteral("left"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- fingersNum ----

TEST(GestureData, SetFingersNumEmitsOnChange)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::fingersNumChanged);

    gd.setFingersNum(3);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.fingersNum(), 3);
}

TEST(GestureData, SetFingersNumNoSignalOnSameValue)
{
    GestureData gd;
    gd.setFingersNum(3);

    QSignalSpy spy(&gd, &GestureData::fingersNumChanged);
    gd.setFingersNum(3);
    EXPECT_EQ(spy.count(), 0);
}

TEST(GestureData, SetFingersNumToZero)
{
    GestureData gd;
    gd.setFingersNum(5);

    QSignalSpy spy(&gd, &GestureData::fingersNumChanged);
    gd.setFingersNum(0);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.fingersNum(), 0);
}

TEST(GestureData, SetFingersNumNegative)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::fingersNumChanged);

    gd.setFingersNum(-1);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.fingersNum(), -1);
}

// ---- actionName ----

TEST(GestureData, SetActionNameEmitsOnChange)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::actionNameChanged);

    gd.setActionName(QStringLiteral("Launch"));
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.actionName(), QStringLiteral("Launch"));
}

TEST(GestureData, SetActionNameNoSignalOnSameValue)
{
    GestureData gd;
    gd.setActionName(QStringLiteral("Launch"));

    QSignalSpy spy(&gd, &GestureData::actionNameChanged);
    gd.setActionName(QStringLiteral("Launch"));
    EXPECT_EQ(spy.count(), 0);
}

// ---- sequence ----

TEST(GestureData, SetSequenceEmitsOnChange)
{
    GestureData gd;
    // Default is -1.
    QSignalSpy spy(&gd, &GestureData::sequenceChanged);

    gd.setSequence(0);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.sequence(), 0);
}

TEST(GestureData, SetSequenceNoSignalOnSameValue)
{
    GestureData gd;
    gd.setSequence(5);

    QSignalSpy spy(&gd, &GestureData::sequenceChanged);
    gd.setSequence(5);
    EXPECT_EQ(spy.count(), 0);
}

TEST(GestureData, SetSequenceFromDefaultToNegative)
{
    GestureData gd;
    // Default is -1; setting -1 again should NOT emit.
    QSignalSpy spy(&gd, &GestureData::sequenceChanged);
    gd.setSequence(-1);
    EXPECT_EQ(spy.count(), 0);
}

TEST(GestureData, SetSequencePositive)
{
    GestureData gd;
    QSignalSpy spy(&gd, &GestureData::sequenceChanged);

    gd.setSequence(42);
    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(gd.sequence(), 42);
}

// ---- actions ----

TEST(GestureData, SetActionsReturnsSetList)
{
    GestureData gd;
    QList<GestureActionData> list;
    GestureActionData a;
    a.actionId = "id1";
    a.displayName = "Name1";
    a.supported = true;
    a.unavailableReason = "";
    list.append(a);

    GestureActionData b;
    b.actionId = "id2";
    b.displayName = "Name2";
    b.supported = false;
    b.unavailableReason = "Not available";
    list.append(b);

    gd.setActions(list);
    QList<GestureActionData> result = gd.actions();
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].actionId, QStringLiteral("id1"));
    EXPECT_EQ(result[0].displayName, QStringLiteral("Name1"));
    EXPECT_TRUE(result[0].supported);
    EXPECT_EQ(result[1].actionId, QStringLiteral("id2"));
    EXPECT_EQ(result[1].displayName, QStringLiteral("Name2"));
    EXPECT_FALSE(result[1].supported);
    EXPECT_EQ(result[1].unavailableReason, QStringLiteral("Not available"));
}

TEST(GestureData, SetActionsOverwritesPrevious)
{
    GestureData gd;
    QList<GestureActionData> first;
    first.append(GestureActionData{"a1", "A1", true, ""});
    gd.setActions(first);
    ASSERT_EQ(gd.actions().size(), 1);

    QList<GestureActionData> second;
    second.append(GestureActionData{"b1", "B1", false, "reason"});
    second.append(GestureActionData{"b2", "B2", true, ""});
    gd.setActions(second);
    ASSERT_EQ(gd.actions().size(), 2);
    EXPECT_EQ(gd.actions()[0].actionId, QStringLiteral("b1"));
    EXPECT_EQ(gd.actions()[1].actionId, QStringLiteral("b2"));
}

TEST(GestureData, SetActionsEmptyList)
{
    GestureData gd;
    QList<GestureActionData> list;
    list.append(GestureActionData{"a1", "A1", true, ""});
    gd.setActions(list);

    gd.setActions(QList<GestureActionData>());
    EXPECT_TRUE(gd.actions().isEmpty());
}

// ---- Multiple property changes in sequence ----

TEST(GestureData, MultipleSettersIndependent)
{
    GestureData gd;
    QSignalSpy spyType(&gd, &GestureData::actionTypeChanged);
    QSignalSpy spyDir(&gd, &GestureData::directionChanged);
    QSignalSpy spyFingers(&gd, &GestureData::fingersNumChanged);

    gd.setActionType("click");
    gd.setDirection("up");
    gd.setFingersNum(4);

    EXPECT_EQ(spyType.count(), 1);
    EXPECT_EQ(spyDir.count(), 1);
    EXPECT_EQ(spyFingers.count(), 1);
    EXPECT_EQ(gd.actionType(), QStringLiteral("click"));
    EXPECT_EQ(gd.direction(), QStringLiteral("up"));
    EXPECT_EQ(gd.fingersNum(), 4);
}
