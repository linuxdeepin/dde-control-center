// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SYSTEMINFOWORK_H
#define SYSTEMINFOWORK_H

#include <QObject>
#include <com_deepin_daemon_systeminfo.h>
#include <com_deepin_daemon_grub2.h>
#include <com_deepin_daemon_grub2_theme.h>
#include <org_freedesktop_hostname1.h>

using SystemInfoInter=com::deepin::daemon::SystemInfo;
using GrubDbus=com::deepin::daemon::Grub2;
using GrubThemeDbus = com::deepin::daemon::grub2::Theme;
using HostNameDbus = org::freedesktop::hostname1;

namespace dcc{
namespace systeminfo{

enum AuthorizationProperty {
    Dedault = 0,  //默认
    Government,  //政务
    Enterprise,  //企业
    Count,
};

class SystemInfoModel;

class SystemInfoWork : public QObject
{
    Q_OBJECT

public:
    explicit SystemInfoWork(SystemInfoModel* model, QObject* parent = 0);

    void activate();
    void deactivate();

    void loadGrubSettings();

Q_SIGNALS:
    void requestSetAutoHideDCC(const bool visible) const;

public Q_SLOTS:
    void setBootDelay(bool value);
    void setEnableTheme(bool value);
    void setDefaultEntry(const QString& entry);
    void grubServerFinished();
    void onBackgroundChanged();
    void setBackground(const QString &path);
    void showActivatorDialog();
    void licenseStateChangeSlot();
    void processChanged(QDBusMessage msg);
    void onLicenseAuthorizationProperty(void);

private:
    void getEntryTitles();
    void getBackgroundFinished(QDBusPendingCallWatcher *w);
    void getLicenseState();
    unsigned int getLicenseAuthorizationProperty();

private:
    SystemInfoModel* m_model;
    SystemInfoInter* m_systemInfoInter;
    GrubDbus* m_dbusGrub;
    GrubThemeDbus *m_dbusGrubTheme;
    HostNameDbus *m_dbusHostName;
    QDBusInterface *m_systemInfo;
};

}
}
#endif // SYSTEMINFOWORK_H
