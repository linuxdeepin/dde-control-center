//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QTime>
#include <QDBusVariant>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class NotificationSetting;
class NotificationWorker;

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

    explicit SysItemModel(NotificationSetting *setting, QObject *parent = nullptr);

    Q_PROPERTY(bool disturbMode READ disturbMode WRITE setDisturbMode NOTIFY disturbModeChanged FINAL)
    Q_PROPERTY(bool lockScreen READ lockScreen WRITE setLockScreen NOTIFY lockScreenChanged FINAL)
    Q_PROPERTY(bool timeSlot READ timeSlot WRITE setTimeSlot NOTIFY timeSlotChanged FINAL)
    Q_PROPERTY(int  bubbleCount READ bubbleCount WRITE setBubbleCount NOTIFY bubbleCountChanged FINAL)
    Q_PROPERTY(QString timeStart READ timeStart WRITE setTimeStart NOTIFY timeStartChanged FINAL)
    Q_PROPERTY(QString timeEnd READ timeEnd WRITE setTimeEnd NOTIFY timeEndChanged FINAL)

    bool disturbMode() const;
    void setDisturbMode(const bool disturbMode);

    bool timeSlot()const;
    void setTimeSlot(const bool timeSlot);

    bool lockScreen()const;
    void setLockScreen(const bool lockScreen);

    QString timeStart()const;
    void setTimeStart(const QString &timeStart);

    QString timeEnd()const;
    void setTimeEnd(const QString &timeEnd);

    int bubbleCount() const;
    void setBubbleCount(int newBubbleCount);

public Q_SLOTS:
    void onSettingChanged(const QString &key);

Q_SIGNALS:
    void disturbModeChanged(bool disturbMode);
    void timeSlotChanged(bool timeSlot);
    void lockScreenChanged(bool lockScreen);
    void timeStartChanged(const QString &timeStart);
    void timeEndChanged(const QString &timeEnd);
    void maxCountChanged(const int maxCount);

    void bubbleCountChanged(const int bubbleCount);

private:
    NotificationSetting *m_setting;
};

}
