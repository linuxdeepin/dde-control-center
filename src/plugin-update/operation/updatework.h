// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "updatemodel.h"

#include <QObject>
#include <QDBusInterface>

const QString UpdateManagerService = QStringLiteral("org.deepin.UpdateManager1");
const QString UpdateManagerPath = QStringLiteral("/org/deepin/UpdateManager1");
const QString UpdateManagerInterface = QStringLiteral("org.deepin.UpdateManager1");
const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

class UpdateWork : public QObject
{
    Q_OBJECT
public:
    explicit UpdateWork(UpdateModel* model, QObject *parent = nullptr);

    Q_INVOKABLE void checkUpgrade();
    Q_INVOKABLE void onActionBtnClicked();
    Q_INVOKABLE void showUpdateCtl(bool isShow);
    Q_INVOKABLE void checkProcessStop();
signals:

private Q_SLOTS:
    void onPropertiesChanged(const QDBusMessage &message);
    void onProcess(QPair<QString, double> processValue);

private:
    void initData();
    void initConnection();

    void upgrade();
    
    UpdateModel* m_model;
    QDBusInterface* m_updateInterface;
};
#endif // UPDATEWORK_H
