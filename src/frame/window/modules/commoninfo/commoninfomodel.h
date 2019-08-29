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
#include "window/namespace.h"

#include <QObject>
#include <QPixmap>

namespace DCC_NAMESPACE {
namespace commoninfo {

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
    inline bool updating() const { return m_updating; }
    QPixmap background() const;
    void setBackground(const QPixmap &background);

Q_SIGNALS:
    void bootDelayChanged(const int timeout) const;
    void themeEnabledChanged(const bool enabled) const;
    void entryListsChanged(const QStringList &list);
    void defaultEntryChanged(const QString &entry);
    void updatingChanged(const bool &updating);
    void backgroundChanged(const QPixmap &pixmap);

public Q_SLOTS:
    void setBootDelay(bool bootDelay);
    void setThemeEnabled(const bool enabled);
    void setDefaultEntry(const QString &entry);
    void setUpdating(bool updating);

private:
    bool m_bootDelay;
    bool m_themeEnabled;
    bool m_updating;
    QStringList m_entryLists;
    QString m_defaultEntry;
    QPixmap m_background;
};

} // namespace commoninfo
} // namespace DCC_NAMESPACE
