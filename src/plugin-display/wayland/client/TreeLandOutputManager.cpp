// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TreeLandOutputManager.h"

#include "WayQtLogging.h"
#include "WayQtUtils.h"

#include <wayland-client.h>

#include <QDebug>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QScreen>

#include "wayland-treeland-output-manager-unstable-v2-client-protocol.h"

// ── WQt::ColorControl ──────────────────────────────────────────────

WQt::ColorControl::ColorControl(::treeland_output_picture_control_v2 *obj, QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::ColorControl>(treeland_output_picture_control_v2_interface.version)
    , QtWayland::treeland_output_picture_control_v2(obj)
{
    setParent(parent);
}

WQt::ColorControl::~ColorControl()
{
    if (isInitialized())
        QtWayland::treeland_output_picture_control_v2::destroy();
}

void WQt::ColorControl::setBrightness(double brightness)
{
    // An out-of-range value is a fatal protocol error that terminates the
    // connection, so clamp before sending.
    if (brightness < 0.0 || brightness > 100.0) {
        qCWarning(DccWayQt) << "clamping brightness" << brightness << "to [0.0, 100.0]";
        brightness = qBound(0.0, brightness, 100.0);
    }
    qCDebug(DccWayQt) << "ColorControl::setBrightness" << brightness;
    QtWayland::treeland_output_picture_control_v2::set_brightness(wl_fixed_from_double(brightness));
    commit();
}

void WQt::ColorControl::setColorTemperature(uint32_t temperature)
{
    // An out-of-range value is a fatal protocol error that terminates the
    // connection, so clamp before sending.
    if (temperature < 1000 || temperature > 20000) {
        qCWarning(DccWayQt) << "clamping color temperature" << temperature << "to [1000, 20000]";
        temperature = qBound<uint32_t>(1000, temperature, 20000);
    }
    qCDebug(DccWayQt) << "ColorControl::setColorTemperature" << temperature;
    QtWayland::treeland_output_picture_control_v2::set_color_temperature(temperature);
    commit();
}

void WQt::ColorControl::treeland_output_picture_control_v2_result(uint32_t commitResult)
{
    // The parameter cannot be named "result": it would shadow the
    // result(uint32_t) signal and break the emit below.
    Q_EMIT result(commitResult);
}

void WQt::ColorControl::treeland_output_picture_control_v2_color_temperature(uint32_t temperature)
{
    Q_EMIT colorTemperatureChanged(temperature);
}

void WQt::ColorControl::treeland_output_picture_control_v2_brightness(int32_t brightness)
{
    Q_EMIT brightnessChanged(wl_fixed_to_double(brightness));
}

// ── WQt::TreeLandOutputManager ─────────────────────────────────────

WQt::TreeLandOutputManager::TreeLandOutputManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::TreeLandOutputManager>(treeland_output_manager_v2_interface.version)
{
    setParent(parent);
}

WQt::TreeLandOutputManager::~TreeLandOutputManager()
{
    if (isInitialized())
        QtWayland::treeland_output_manager_v2::destroy();
}

void WQt::TreeLandOutputManager::setPrimaryOutput(struct wl_output *output)
{
    qCDebug(DccWayQt) << "TreeLandOutputManager::setPrimaryOutput" << output;
    QtWayland::treeland_output_manager_v2::set_primary_output(output);
}

WQt::ColorControl *WQt::TreeLandOutputManager::getPictureControl(struct wl_output *output)
{
    if (!output)
        return nullptr;

    // Same guard as WallpaperManager::getWallpaper: a removed output turns this
    // request into a fatal protocol error.
    if (!WQt::Utils::isOutputAlive(output)) {
        qCWarning(DccWayQt) << "skipping get_picture_control for a removed output" << output;
        return nullptr;
    }

    auto *pictureControl = get_picture_control(output);
    if (!pictureControl)
        return nullptr;

    return new WQt::ColorControl(pictureControl, this);
}

void WQt::TreeLandOutputManager::treeland_output_manager_v2_primary_output(struct wl_output *output)
{
    QString name;
    if (output) {
        auto *screen = WQt::Utils::qScreenFromWlOutput(output);
        if (screen)
            name = screen->name();
    }
    qCDebug(DccWayQt) << "TreeLandOutputManager::primary output changed" << name;
    mPrimaryOutput = name;
    Q_EMIT primaryOutputChanged(name);
}

void WQt::TreeLandOutputManager::treeland_output_manager_v2_primary_output_failed(uint32_t reason)
{
    qCWarning(DccWayQt) << "TreeLandOutputManager::set_primary_output rejected, reason" << reason;
}
