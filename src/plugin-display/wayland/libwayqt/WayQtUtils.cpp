// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "WayQtUtils.h"

#include <qpa/qplatformnativeinterface.h>
#include <wayland-client.h>

#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

wl_display *WQt::Wayland::display()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_display *display =
            reinterpret_cast<wl_display *>(native->nativeResourceForIntegration("display"));

    return display;
}

wl_compositor *WQt::Wayland::compositor()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_compositor *display =
            reinterpret_cast<wl_compositor *>(native->nativeResourceForIntegration("compositor"));

    return display;
}

wl_seat *WQt::Wayland::seat()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_seat *display =
            reinterpret_cast<wl_seat *>(native->nativeResourceForIntegration("wl_seat"));

    return display;
}

wl_pointer *WQt::Wayland::pointer()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_pointer *display =
            reinterpret_cast<wl_pointer *>(native->nativeResourceForIntegration("wl_pointer"));

    return display;
}

wl_keyboard *WQt::Wayland::keyboard()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_keyboard *display =
            reinterpret_cast<wl_keyboard *>(native->nativeResourceForIntegration("wl_keyboard"));

    return display;
}

wl_touch *WQt::Wayland::touch()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_touch *display =
            reinterpret_cast<wl_touch *>(native->nativeResourceForIntegration("wl_touch"));

    return display;
}

void WQt::Utils::flushDisplay()
{
    wl_display_flush(WQt::Wayland::display());
}

void WQt::Utils::displayRoundtrip()
{
    wl_display_roundtrip(WQt::Wayland::display());
}

wl_output *WQt::Utils::wlOutputFromQScreen(QScreen *screen)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_output *output =
            reinterpret_cast<wl_output *>(native->nativeResourceForScreen("output", screen));

    return output;
}

wl_surface *WQt::Utils::wlSurfaceFromQWindow(QWindow *window)
{
    window->create();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native) {
        return nullptr;
    }

    struct wl_surface *surface =
            reinterpret_cast<wl_surface *>(native->nativeResourceForWindow("surface", window));

    return surface;
}

bool WQt::Utils::isWayland()
{
    /* Check if XDG_SESSION_TYPE is set */
    QString session = qgetenv("XDG_SESSION_TYPE");

    if (session.toLower() == QStringLiteral("wayland")) {
        return true;
    }

    /*
     * May be XDG_SESSION_TYPE is not set. Try Harder.
     * We check if WAYLAND_DISPLAY is set.
     */
    QString wlID = qgetenv("WAYLAND_DISPLAY");

    if (!wlID.isEmpty()) {
        return true;
    }

    /* It's probably not a wayland session. */
    return false;
}

bool WQt::Utils::isTreeland()
{
    /** Checking the WAYFIRE_CONFIG_FILE variable is sufficient for our cause */
    static auto diff = qgetenv("DDE_CURRENT_COMPOSITOR").compare("TreeLand", Qt::CaseInsensitive);

    return diff == 0;
}
