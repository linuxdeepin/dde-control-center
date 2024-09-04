//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMINFOWORK_H
#define SYSTEMINFOWORK_H

#include <qprocess.h>

#include <QDBusMessage>
#include <QObject>


class QApplication;
class SystemInfoDBusProxy;
namespace DCC_NAMESPACE{

class SystemInfoModel;
class SystemInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoWork(SystemInfoModel* model, QObject* parent = nullptr);
    virtual ~SystemInfoWork();

    void activate();
    void deactivate();

    //void loadGrubSettings();
    QString getEndUserAgreementText();

    void initGnuLicenseData();
    void initUserLicenseData();
    void initSystemCopyright();


    Q_INVOKABLE void setUeProgram(bool enabled);
    Q_INVOKABLE void showActivatorDialog();
    Q_INVOKABLE void showDetail();

Q_SIGNALS:
    void requestSetAutoHideDCC(const bool visible) const;

public Q_SLOTS:

    void onSetHostname(const QString &hostname);
    void onSetHostnameFinish();

private:
    void getLicenseState();

private:
    SystemInfoModel *m_model;
    SystemInfoDBusProxy *m_systemInfDBusProxy;
    QProcess *m_process = nullptr;
    QString m_content;
    QString m_title;
};

}
#endif // SYSTEMINFOWORK_H
