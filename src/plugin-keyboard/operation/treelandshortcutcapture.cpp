// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "treelandshortcutcapture.h"

#include <QDebug>
#include <QGuiApplication>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>

extern "C" {
#include <wayland-client-core.h>
}

namespace dccV25 {

TreelandShortcutCaptureSession::TreelandShortcutCaptureSession(
    struct ::treeland_shortcut_capture_v3 *obj, QObject *parent)
    : QObject(parent)
    , QtWayland::treeland_shortcut_capture_v3(obj)
{
}

TreelandShortcutCaptureSession::~TreelandShortcutCaptureSession()
{
    if (!m_finished && isInitialized()) {
        // Destroying while pending cancels the capture per protocol.
        QtWayland::treeland_shortcut_capture_v3::destroy();
    }
}

void TreelandShortcutCaptureSession::treeland_shortcut_capture_v3_captured(const QString &key)
{
    m_finished = true;
    emit captured(key);
    QtWayland::treeland_shortcut_capture_v3::destroy();
    deleteLater();
}

void TreelandShortcutCaptureSession::treeland_shortcut_capture_v3_failed(uint32_t reason)
{
    m_finished = true;
    emit failed(reason);
    QtWayland::treeland_shortcut_capture_v3::destroy();
    deleteLater();
}

TreelandShortcutCaptureManager::TreelandShortcutCaptureManager(QObject *parent)
    : QWaylandClientExtensionTemplate<TreelandShortcutCaptureManager>(1)
{
    setParent(parent);
    initialize();
}

TreelandShortcutCaptureManager::~TreelandShortcutCaptureManager()
{
    // The v3 manager global exposes interface version 1. Destroy it only
    // after the extension has bound the advertised global.
    if (isActive()) {
        QtWayland::treeland_shortcut_manager_v3::destroy();
    }
}

TreelandShortcutCaptureSession *TreelandShortcutCaptureManager::captureNext(QWindow *window)
{
    if (!isActive()) {
        qWarning() << "TreelandShortcutCaptureManager: protocol not active";
        return nullptr;
    }
    if (!window) {
        qWarning() << "TreelandShortcutCaptureManager: null window";
        return nullptr;
    }

    QPlatformNativeInterface *iface = QGuiApplication::platformNativeInterface();
    if (!iface) {
        qWarning() << "TreelandShortcutCaptureManager: no platform native interface";
        return nullptr;
    }
    auto *surface = static_cast<struct ::wl_surface *>(
        iface->nativeResourceForWindow(QByteArrayLiteral("surface"), window));
    if (!surface) {
        qWarning() << "TreelandShortcutCaptureManager: window has no wl_surface" << window;
        return nullptr;
    }

    struct ::treeland_shortcut_capture_v3 *obj = capture_next_shortcut(surface, nullptr);
    if (!obj) {
        qWarning() << "TreelandShortcutCaptureManager: capture_next_shortcut returned null";
        return nullptr;
    }
    return new TreelandShortcutCaptureSession(obj, this);
}

} // namespace dccV25
