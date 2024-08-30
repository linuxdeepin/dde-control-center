//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QObject>
#include <QTime>
#include <QDBusVariant>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class SysItemModel : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        DNDMODE,
        LOCKSCREENOPENDNDMODE,
        OPENBYTIMEINTERVAL,
        STARTTIME,
        ENDTIME,
        SHOWICON
    } SystemConfigurationItem;

    explicit SysItemModel(QObject *parent = nullptr);

    inline bool isDisturbMode() const {return m_isDisturbMode;}
    void setDisturbMode(const bool disturbMode);

    inline bool isTimeSlot()const {return  m_isTimeSlot;}
    void setTimeSlot(const bool timeSlot);

    inline bool isLockScreen()const {return m_isLockScreen;}
    void setLockScreen(const bool lockScreen);

    inline QString timeStart()const {return m_timeStart;}
    void setTimeStart(const QString &timeStart);

    inline QString timeEnd()const {return m_timeEnd;}
    void setTimeEnd(const QString &timeEnd);

    void onSettingChanged(uint item, const QDBusVariant &var);

Q_SIGNALS:
    void disturbModeChanged(bool isDisturbMode);
    void timeSlotChanged(bool isTimeSlot);
    void lockScreenChanged(bool isLockScreen);
    void timeStartChanged(const QString &timeStart);
    void timeEndChanged(const QString &timeEnd);

private:
    bool m_isDisturbMode;//勿扰模式
    bool m_isTimeSlot;//时间段
    bool m_isLockScreen;//锁屏显示
    QString m_timeStart;//开始时间
    QString m_timeEnd;//结束时间
};

}
