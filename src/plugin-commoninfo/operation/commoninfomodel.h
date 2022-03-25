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

#include <QObject>
#include <QPixmap>

namespace DCC_NAMESPACE {
class CommonInfoModel : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoModel(QObject *parent = nullptr);

    void setEntryLists(const QStringList &list);
    inline QStringList entryLists() const { return m_entryLists;}
    inline QString defaultEntry() const { return m_defaultEntry;}
    bool bootDelay() const;
    inline bool themeEnabled() const { return m_themeEnabled; }
    inline bool isShowGrubEditAuth() { return m_isShowGrubEditAuth; }
    inline bool grubEditAuthEnabled() { return m_grubEditAuthEnabled; }
    inline bool updating() const { return m_updating; }
    QPixmap background() const;
    void setBackground(const QPixmap &background);
    bool ueProgram() const; // for user experience program
    bool developerModeState() const;
    inline bool isLogin() const { return m_isLogin; }
    inline bool isActivate() const { return m_activation; }
    void setActivation(bool value);

Q_SIGNALS:
    void bootDelayChanged(const bool enabled) const;
    void themeEnabledChanged(const bool enabled) const;
    void grubEditAuthEnabledChanged(const bool enabled) const;
    void entryListsChanged(const QStringList &list);
    void defaultEntryChanged(const QString &entry);
    void updatingChanged(const bool &updating);
    void backgroundChanged(const QPixmap &pixmap);
    void ueProgramChanged(const bool enable) const; // for user experience program
    void developerModeStateChanged(const bool enable) const;
    void isLoginChenged(bool log) const;
    void LicenseStateChanged(bool state);

public Q_SLOTS:
    void setBootDelay(bool bootDelay);
    void setThemeEnabled(const bool enabled);
    void setShowGrubEditAuth(const bool enabled);
    void setGrubEditAuthEnabled(const bool enabled);
    void setDefaultEntry(const QString &entry);
    void setUpdating(bool updating);
    void setUeProgram(const bool ueProgram); // for user experience program
    void setDeveloperModeState(const bool state);
    void setIsLogin(const bool log);

private:
    bool m_bootDelay;
    bool m_themeEnabled;
    bool m_isShowGrubEditAuth = false;
    bool m_grubEditAuthEnabled = false;
    bool m_updating;
    QStringList m_entryLists;
    QString m_defaultEntry;
    QPixmap m_background;
    bool m_joinUeProgram;   // for user experience program
    bool m_developerModeState{false}; // for developer mode state
    bool m_isLogin{false};
    bool m_activation;
};
} // namespace DCC_NAMESPACE
