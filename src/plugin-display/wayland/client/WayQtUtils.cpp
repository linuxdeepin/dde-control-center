// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "WayQtUtils.h"

#include "WayQtLogging.h"

#include <QGuiApplication>
#include <QScreen>
#include <QtGui/qguiapplication_platform.h>
#include <QtGui/qscreen_platform.h>

#include <wayland-client-core.h>

Q_LOGGING_CATEGORY(DccWayQt, "dcc-wayqt")

wl_output *WQt::Utils::wlOutputFromQScreen(QScreen *screen)
{
    if (!screen)
        return nullptr;

    auto *native = screen->nativeInterface<QNativeInterface::QWaylandScreen>();
    if (!native)
        return nullptr;

    return native->output();
}

bool WQt::Utils::isOutputAlive(wl_output *output)
{
    if (!output)
        return false;

    // QWaylandDisplay drops the QScreen when it handles wl_registry.global_remove
    // for the output, which is exactly when the compositor stops accepting
    // requests naming it.
    for (QScreen *screen : QGuiApplication::screens()) {
        if (wlOutputFromQScreen(screen) == output)
            return true;
    }

    return false;
}

bool WQt::Utils::isTreeland()
{
    static auto diff = qgetenv("DDE_CURRENT_COMPOSITOR").compare("TreeLand", Qt::CaseInsensitive);

    return diff == 0;
}
