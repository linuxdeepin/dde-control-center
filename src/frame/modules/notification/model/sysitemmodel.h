/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "interface/namespace.h"

#include <QObject>
#include <QTime>
#include <QDBusVariant>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace dcc {
namespace notification {

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

    inline bool isShowInDock()const {return m_isShowInDock;}
    void setShowInDock(const bool showInDock);

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
    void showInDockChanged(bool isShowInDock);
    void timeSlotChanged(bool isTimeSlot);
    void lockScreenChanged(bool isLockScreen);
    void timeStartChanged(const QString &timeStart);
    void timeEndChanged(const QString &timeEnd);

private:
    bool m_isDisturbMode;//勿扰模式
    bool m_isShowInDock;//任务栏显示
    bool m_isTimeSlot;//时间段
    bool m_isLockScreen;//锁屏显示
    QString m_timeStart;//开始时间
    QString m_timeEnd;//结束时间
};

}// namespace dcc
}// namespace msgnotify
