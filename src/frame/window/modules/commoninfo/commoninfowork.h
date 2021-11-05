/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include <com_deepin_daemon_systeminfo.h>
#include <com_deepin_daemon_grub2.h>
#include <com_deepin_daemon_grub2_theme.h>
#include <com_deepin_daemon_grub2_editauthentication.h>
#include <com_deepin_system_userexperience_daemon.h>
#include <com_deepin_deepinid.h>

#include <QObject>

#include <DObject>

using GrubDbus = com::deepin::daemon::Grub2;
using GrubThemeDbus = com::deepin::daemon::grub2::Theme;
using GrubEditAuthDbus = com::deepin::daemon::grub2::EditAuthentication;
using UeProgramDbus = com::deepin::userexperience::Daemon;
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
    bool defaultUeProgram();
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

private:
    CommonInfoModel *m_commomModel;
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
    GrubDbus *m_dBusGrub;
    GrubThemeDbus *m_dBusGrubTheme;
    GrubEditAuthDbus *m_dBusGrubEditAuth;
    UeProgramDbus *m_dBusUeProgram; // for user experience program
    QProcess *m_process = nullptr;
    GrubDevelopMode *m_dBusdeepinIdInter;
    QString m_title;
    QString m_content;
};
} // namespace commoninfo
} // namespace DCC_NAMESPACE
