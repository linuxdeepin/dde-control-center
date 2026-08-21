// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TreeLandOutputManager.h"

#include "WayQtLogging.h"

#include <wayland-client.h>

#include <QDebug>
#include <QLoggingCategory>

// ── WQt::ColorControl ──────────────────────────────────────────────

WQt::ColorControl::ColorControl(::treeland_output_color_control_v1 *obj, QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::ColorControl>(1)
    , QtWayland::treeland_output_color_control_v1(obj)
{
    setParent(parent);
}

WQt::ColorControl::~ColorControl()
{
    QtWayland::treeland_output_color_control_v1::destroy();
}

void WQt::ColorControl::setBrightness(double brightness)
{
    qCDebug(DccWayQt) << "ColorControl::setBrightness" << brightness;
    QtWayland::treeland_output_color_control_v1::set_brightness(wl_fixed_from_double(brightness));
    commit();
}

void WQt::ColorControl::setColorTemperature(uint32_t temperature)
{
    qCDebug(DccWayQt) << "ColorControl::setColorTemperature" << temperature;
    QtWayland::treeland_output_color_control_v1::set_color_temperature(temperature);
    commit();
}

void WQt::ColorControl::treeland_output_color_control_v1_result(uint32_t success)
{
    Q_EMIT result(success);
}

void WQt::ColorControl::treeland_output_color_control_v1_color_temperature(uint32_t temperature)
{
    Q_EMIT colorTemperatureChanged(temperature);
}

void WQt::ColorControl::treeland_output_color_control_v1_brightness(int32_t brightness)
{
    Q_EMIT brightnessChanged(wl_fixed_to_double(brightness));
}

// ── WQt::TreeLandOutputManager ─────────────────────────────────────

WQt::TreeLandOutputManager::TreeLandOutputManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::TreeLandOutputManager>(2)
{
    setParent(parent);
}

WQt::TreeLandOutputManager::~TreeLandOutputManager()
{
    QtWayland::treeland_output_manager_v1::destroy();
}

void WQt::TreeLandOutputManager::setPrimaryOutput(const char *name)
{
    qCDebug(DccWayQt) << "TreeLandOutputManager::setPrimaryOutput" << name;
    QtWayland::treeland_output_manager_v1::set_primary_output(name);
}

WQt::ColorControl *WQt::TreeLandOutputManager::getColorControl(struct wl_output *output)
{
    auto *colorControl = get_color_control(output);
    if (!colorControl)
        return nullptr;
    return new WQt::ColorControl(colorControl, this);
}

void WQt::TreeLandOutputManager::treeland_output_manager_v1_primary_output(const QString &output_name)
{
    qCDebug(DccWayQt) << "TreeLandOutputManager::primary output changed" << output_name;
    mPrimaryOutput = output_name;
    Q_EMIT primaryOutputChanged(output_name);
}
