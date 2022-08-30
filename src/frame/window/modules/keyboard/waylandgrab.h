// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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