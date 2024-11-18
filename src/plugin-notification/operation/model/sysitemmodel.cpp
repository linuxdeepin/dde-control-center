//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "sysitemmodel.h"
#include "../notificationsetting.h"

using namespace DCC_NAMESPACE;

SysItemModel::SysItemModel(NotificationSetting *setting, QObject *parent)
    : QObject(parent)
    , m_setting(setting)
{
    connect(m_setting, SIGNAL(systemValueChanged(const QString&)), this, SLOT(onSettingChanged(const QString&)));
}

void SysItemModel::setDisturbMode(const bool disturbMode)
{
    m_setting->setSystemValue(NotificationSetting::DNDMode, disturbMode);
    Q_EMIT disturbModeChanged(disturbMode);
}

bool SysItemModel::timeSlot() const
{
    return m_setting->systemValue(NotificationSetting::OpenByTimeInterval).toBool();
}

void SysItemModel::setTimeSlot(const bool timeSlot)
{
    m_setting->setSystemValue(NotificationSetting::OpenByTimeInterval, timeSlot);
    Q_EMIT timeSlotChanged(timeSlot);
}

bool SysItemModel::lockScreen() const
{
    return m_setting->systemValue(NotificationSetting::LockScreenOpenDNDMode).toBool();
}

void SysItemModel::setLockScreen(const bool lockScreen)
{
    m_setting->setSystemValue(NotificationSetting::LockScreenOpenDNDMode, lockScreen);
    Q_EMIT lockScreenChanged(lockScreen);
}

QString SysItemModel::timeStart() const
{
    return m_setting->systemValue(NotificationSetting::StartTime).toString();
}

void SysItemModel::setTimeStart(const QString &timeStart)
{
    m_setting->setSystemValue(NotificationSetting::StartTime, timeStart);
    Q_EMIT timeStartChanged(timeStart);
}

QString SysItemModel::timeEnd() const
{
    return m_setting->systemValue(NotificationSetting::EndTime).toString();
}

void SysItemModel::setTimeEnd(const QString &timeEnd)
{
    m_setting->setSystemValue(NotificationSetting::EndTime, timeEnd);
    Q_EMIT timeEndChanged(timeEnd);
}

void SysItemModel::onSettingChanged(const QString &key)
{
    if ("dndMode" == key) {
        Q_EMIT disturbModeChanged(m_setting->systemValue(NotificationSetting::DNDMode).toBool());
    } else if ("lockScreenOpenDndMode" == key) {
        Q_EMIT lockScreenChanged(m_setting->systemValue(NotificationSetting::LockScreenOpenDNDMode).toBool());
    } else if ("openByTimeInterval" == key) {
        Q_EMIT timeSlotChanged(m_setting->systemValue(NotificationSetting::OpenByTimeInterval).toBool());
    } else if ("startTime" == key) {
        Q_EMIT timeStartChanged(m_setting->systemValue(NotificationSetting::StartTime).toString());
    } else if ("endTime" == key) {
        Q_EMIT timeEndChanged(m_setting->systemValue(NotificationSetting::EndTime).toString());
    } else if ("bubbleCount" == key) {
        Q_EMIT bubbleCountChanged(m_setting->systemValue(NotificationSetting::BubbleCount).toInt());
    }
}

bool DCC_NAMESPACE::SysItemModel::disturbMode() const
{
    return m_setting->systemValue(NotificationSetting::DNDMode).toBool();
}

int SysItemModel::bubbleCount() const
{
    return m_setting->systemValue(NotificationSetting::BubbleCount).toInt();
}

void SysItemModel::setBubbleCount(int newBubbleCount)
{
    m_setting->setSystemValue(NotificationSetting::BubbleCount, newBubbleCount);
    Q_EMIT bubbleCountChanged(newBubbleCount);
}
