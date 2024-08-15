//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "notificationmodel.h"
#include "notificationdbusproxy.h"

#include <QObject>

namespace DCC_NAMESPACE {

class NotificationModel;
class NotificationWorker : public QObject
{
    Q_OBJECT
public:
    explicit NotificationWorker(NotificationModel *model, QObject *parent = nullptr);
    void active(bool sync);
    void deactive();
    NotificationDBusProxy *getDbusObject() { return m_dbus; }

public Q_SLOTS:
    void initAllSetting();
    void initSystemSetting();
    void initAppSetting();
    void onAppAdded(const QString &id);
    void onAppRemoved(const QString &id);
    void setAppSetting(const QString &id, uint item, QVariant var);
    void setSystemSetting(uint item, QVariant var);

private:
    NotificationModel *m_model;
    NotificationDBusProxy *m_dbus;
};
}
