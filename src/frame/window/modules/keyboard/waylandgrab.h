/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     chenbin <chenbina@deepin.com>
 *
 * Maintainer: chenbin <chenbina@deepin.com>
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
#ifndef WAYLANDGRAB_H
#define WAYLANDGRAB_H

#include "wayland-xwayland-keyboard-grab-v1-client-protocol.h"
#include "modules/keyboard/shortcutmodel.h"

class WaylandGrab : public QObject
{
    Q_OBJECT
public:
    explicit WaylandGrab(QObject *parent = nullptr);
    ~WaylandGrab();

private:
    QString m_lastKey, m_keyValue;
    bool m_record;
    dcc::keyboard::ShortcutInfo *m_info;

    struct ::wl_seat*  m_wlSeat;
    struct ::wl_surface* m_wlSurface;
    struct zwp_xwayland_keyboard_grab_v1 *m_zxgm;

public:
    QString getLastKey() { return m_lastKey; }
    QString getKeyValue() { return m_keyValue; }
    bool getRecordState() { return m_record; }
    void setLastKey(QString key) { m_lastKey = key; }
    void setKeyValue(QString key) { m_keyValue = key; }
    void setRecordState(bool state) { m_record = state; }
    dcc::keyboard::ShortcutInfo *getInfo() { return m_info; }
    struct zwp_xwayland_keyboard_grab_v1 *getZxgm() { return m_zxgm; }

public Q_SLOTS:
    void onGrab(dcc::keyboard::ShortcutInfo *info);
    void onUnGrab();
};




#endif // WAYLANDGRAB_Hs