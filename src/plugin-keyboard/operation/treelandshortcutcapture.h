// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>
#include <QWaylandClientExtension>

#include "qwayland-treeland-shortcut-manager-v2.h"

class QWindow;

namespace dccV25 {

// One-shot session created by capture_next_shortcut. The compositor will
// emit either `captured` (key bound to the next valid combo) or `failed`
// (focus lost, interrupted, busy, aborted). After the terminal event the
// wayland object is inert; the session destroys itself.
class TreelandShortcutCaptureSession
    : public QObject
    , public QtWayland::treeland_shortcut_capture_v1
{
    Q_OBJECT
public:
    explicit TreelandShortcutCaptureSession(struct ::treeland_shortcut_capture_v1 *obj,
                                            QObject *parent = nullptr);
    ~TreelandShortcutCaptureSession() override;

signals:
    void captured(const QString &key);
    void failed(uint32_t reason);

protected:
    void treeland_shortcut_capture_v1_captured(const QString &key) override;
    void treeland_shortcut_capture_v1_failed(uint32_t reason) override;

private:
    bool m_finished = false;
};

// Wraps the treeland_shortcut_manager_v2 global for the capture-only use
// case. The dde-services plugin already holds the manager's acquire lock
// for binding shortcuts; capture_next_shortcut does NOT require acquire
// per protocol, so this client only sends capture requests.
class TreelandShortcutCaptureManager
    : public QWaylandClientExtensionTemplate<TreelandShortcutCaptureManager>
    , public QtWayland::treeland_shortcut_manager_v2
{
    Q_OBJECT
public:
    explicit TreelandShortcutCaptureManager(QObject *parent = nullptr);
    ~TreelandShortcutCaptureManager() override;

    // Returns nullptr if the protocol is not active or the window lacks a
    // wl_surface. The returned session is parented to the manager and
    // self-destructs after captured/failed.
    TreelandShortcutCaptureSession *captureNext(QWindow *window);
};

} // namespace dccV25
