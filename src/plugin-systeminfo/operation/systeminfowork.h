//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMINFOWORK_H
#define SYSTEMINFOWORK_H

#include <qprocess.h>

#include <QDBusInterface>
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
    void updateFrequency(bool state);
    QString getSystemInstallDate(int shortDateFormat, QString timezone);

    Q_INVOKABLE void setUeProgram(bool enabled);
    Q_INVOKABLE void showActivatorDialog();
    Q_INVOKABLE void showDetail();
    Q_INVOKABLE void copyTextToClipboard(const QString &text); // Add new invokable method
    Q_INVOKABLE void playSystemSound(int soundType);

    bool isUeProgramEnabled();
    void setUeProgramEnabled(bool enabled);
    QString getLicenseAuthorizationPropertyString();

Q_SIGNALS:
    void requestSetAutoHideDCC(const bool visible) const;
    void requestUeProgram(bool visible) const;

public Q_SLOTS:
    void onSetHostname(const QString &hostname);
    void onSetHostnameFinish();
    void onTimezoneChanged(const QString timezone);
    void onShortDateFormatChanged(const int shortDateFormate);
    void onLicenseAuthorizationProperty();
    void onThemeTypeChanged();

private:
    void getLicenseState();
    void updateUserExperienceProgramText();

private:
    SystemInfoModel *m_model;
    SystemInfoDBusProxy *m_systemInfDBusProxy;
    QProcess *m_process = nullptr;
    QString m_content;
    QString m_title;
    QDBusInterface *m_dBusUeProgram; // for user experience program

};

}
#endif // SYSTEMINFOWORK_H
