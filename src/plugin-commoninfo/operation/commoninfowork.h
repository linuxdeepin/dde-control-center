/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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

#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QObject>
#include <QProcess>

namespace DCC_NAMESPACE {

static const QStringList SYSTEM_LOCAL_LIST {
    "zh_CN",
    "zh_HK",
    "zh_TW",
    "ug_CN",    // 维语
    "bo_CN"     // 藏语
};

static const QMap<QString, QString> SYSTEM_LOCAL_MAP {
    {"zh_CN", "zh_CN"},
    {"zh_HK", "zh_HK"},
    {"zh_TW", "zh_TW"},
};


class CommonInfoModel;

class CommonInfoWork : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoWork(CommonInfoModel *model, QObject *parent = nullptr);
    virtual ~CommonInfoWork();

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
    void setUeProgram(bool enabled);
    void setEnableDeveloperMode(bool enabled);
    void login();
    void licenseStateChangeSlot();
    void deepinIdErrorSlot(int code, const QString &msg);
    void onDeepinIdPathPropertiesChanged(QDBusMessage msg);
    void onGrubPathPropertiesChanged(QDBusMessage msg);
    void onGrubEditAuthPathPropertiesChanged(QDBusMessage msg);
    void onGrubThemePathPropertiesChanged(QDBusMessage msg);

private:
    void getEntryTitles();
    void getBackgroundFinished(QDBusPendingCallWatcher *w);
    QString passwdEncrypt(const QString &password);

private:
    CommonInfoModel *m_commomModel;

    QDBusInterface *m_dBusGrubInter;
    QDBusInterface *m_dBusGrubPropertiesInter;
    QDBusInterface *m_dBusGrubThemeInter;
    QDBusInterface *m_dBusGrubThemePropertiesInter;
    QDBusInterface *m_dBusGrubEditAuthInter;
    QDBusInterface *m_dBusGrubEditAuthPropertiesInter;
    QDBusInterface *m_dBusUeProgramInter; // for user experience program
    QDBusInterface *m_dBusUeProgramPropertiesInter;
    QDBusInterface *m_dBusdeepinIdInter;
    QDBusInterface *m_dBusdeepinIdPropertiesInter;
    QProcess *m_process = nullptr;
    QString m_title;
    QString m_content;
};
} // namespace DCC_NAMESPACE
