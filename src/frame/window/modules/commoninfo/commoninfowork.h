// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"

#include <com_deepin_daemon_systeminfo.h>
#include <com_deepin_daemon_grub2.h>
#include <com_deepin_daemon_grub2_theme.h>
#include <com_deepin_daemon_grub2_editauthentication.h>
#include <com_deepin_deepinid.h>

#include <QObject>
#include <QDBusInterface>

#include <DObject>

using GrubDbus = com::deepin::daemon::Grub2;
using GrubThemeDbus = com::deepin::daemon::grub2::Theme;
using GrubEditAuthDbus = com::deepin::daemon::grub2::EditAuthentication;
using GrubDevelopMode = com::deepin::deepinid;

DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE

namespace DCC_NAMESPACE {
class MainWindow;
namespace commoninfo {
class CommonInfoModel;

class CommonInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoWork(CommonInfoModel *model, QObject *parent = nullptr);
    virtual ~CommonInfoWork();

    void activate();
    void deactivate();

    void loadGrubSettings();
    bool isUeProgramEnabled();
    void getLicenseState();

Q_SIGNALS:
    void grubEditAuthCancel(bool toEnable);
    void showGrubEditAuthChanged(bool show);

public Q_SLOTS:
    void setBootDelay(bool value);
    void setEnableTheme(bool value);
    void setDefaultEntry(const QString &entry);
    void disableGrubEditAuth();
    void onSetGrubEditPasswd(const QString &password, const bool &isReset);
    void grubServerFinished();
    void onBackgroundChanged();
    void onEnabledUsersChanged(const QStringList &value);
    void setBackground(const QString &path);
    void setUeProgram(bool enabled, DCC_NAMESPACE::MainWindow *pMainWindow);
    void setEnableDeveloperMode(bool enabled, DCC_NAMESPACE::MainWindow *pMainWindow);
    void login();
    void licenseStateChangeSlot();

private:
    void getEntryTitles();
    void getBackgroundFinished(QDBusPendingCallWatcher *w);
    QString passwdEncrypt(const QString &password);
    void setUeProgramEnabled(bool enabled);

private:
    CommonInfoModel *m_commonModel;
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
    GrubDbus *m_dBusGrub;
    GrubThemeDbus *m_dBusGrubTheme;
    GrubEditAuthDbus *m_dBusGrubEditAuth;
    QDBusInterface *m_dBusUeProgram; // for user experience program
    QProcess *m_process;
    GrubDevelopMode *m_deepinIdInter;
    QString m_title;
    QString m_content;
};
} // namespace commoninfo
} // namespace DCC_NAMESPACE
