//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMINFOWORK_H
#define SYSTEMINFOWORK_H

#include <interface/namespace.h>
#include <QObject>
#include <QDBusMessage>

class SystemInfoDBusProxy;
namespace DCC_NAMESPACE{

class SystemInfoModel;
class SystemInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoWork(SystemInfoModel* model, QObject* parent = nullptr);

    void activate();
    void deactivate();

    //void loadGrubSettings();
    QPair<QString, QString> getGNULicenseText();
    QString getEndUserAgreementText();

Q_SIGNALS:
    void requestSetAutoHideDCC(const bool visible) const;

public Q_SLOTS:
    void showActivatorDialog();
    void onSetHostname(const QString &hostname);
    void onSetHostnameFinish();

private:
    void getLicenseState();

private:
    SystemInfoModel *m_model;
    SystemInfoDBusProxy *m_systemInfDBusProxy;
};

}
#endif // SYSTEMINFOWORK_H
