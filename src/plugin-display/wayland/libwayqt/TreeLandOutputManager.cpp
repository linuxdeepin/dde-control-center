// SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TreeLandOutputManager.h"

#include "treeland-output-management-client-protocol.h"

#include <wayland-client.h>

#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QThread>

WQt::TreeLandOutputManager::TreeLandOutputManager(treeland_output_manager_v1 *opMgr)
{
    mObj = opMgr;
    treeland_output_manager_v1_add_listener(mObj, &mListener, this);
}

WQt::TreeLandOutputManager::~TreeLandOutputManager()
{
    treeland_output_manager_v1_destroy(mObj);
}

void WQt::TreeLandOutputManager::setPrimaryOutput(const char *name)
{
    treeland_output_manager_v1_set_primary_output(mObj, name);
}

treeland_output_color_control_v1 *WQt::TreeLandOutputManager::getColorControl(struct wl_output *output)
{
    auto colorControl = treeland_output_manager_v1_get_color_control(mObj, output);
    if (colorControl) {
        treeland_output_color_control_v1_add_listener(colorControl, &mColorControlListener, this);
    }
    return colorControl;
}

void WQt::TreeLandOutputManager::setBrightness(treeland_output_color_control_v1 *control, const double brightness)
{
    if (control) {
        treeland_output_color_control_v1_set_brightness(control, wl_fixed_from_double(brightness));
        treeland_output_color_control_v1_commit(control);
    }
}

void WQt::TreeLandOutputManager::destroyColorControl(treeland_output_color_control_v1 *treeland_output_color_control_v1)
{
    treeland_output_color_control_v1_destroy(treeland_output_color_control_v1);
}

void WQt::TreeLandOutputManager::handlePrimaryOutput(void *data, struct treeland_output_manager_v1 *treeland_output_manager_v1, const char *output_name)
{
    Q_UNUSED(treeland_output_manager_v1)
    WQt::TreeLandOutputManager *manager = reinterpret_cast<WQt::TreeLandOutputManager *>(data);
    manager->mPrimaryOutput = QString::fromLocal8Bit(output_name);
    emit manager->primaryOutputChanged(output_name);
}

void WQt::TreeLandOutputManager::handleResult(void *data, treeland_output_color_control_v1 *treeland_output_color_control_v1, uint32_t success)
{
    // TODO: handleResult
}

void WQt::TreeLandOutputManager::handleColorTemperature(void *data, treeland_output_color_control_v1 *treeland_output_color_control_v1, uint32_t temperature)
{
    WQt::TreeLandOutputManager *manager = reinterpret_cast<WQt::TreeLandOutputManager *>(data);
    emit manager->colorTemperatureChanged(treeland_output_color_control_v1, temperature);
}

void WQt::TreeLandOutputManager::handleBrightness(void *data, struct treeland_output_color_control_v1 *treeland_output_color_control_v1, wl_fixed_t brightness)
{
    WQt::TreeLandOutputManager *manager = reinterpret_cast<WQt::TreeLandOutputManager *>(data);
    double brightnessValue = wl_fixed_to_double(brightness);
    emit manager->brightnessChanged(treeland_output_color_control_v1, brightnessValue);
}

const treeland_output_manager_v1_listener WQt::TreeLandOutputManager::mListener = { handlePrimaryOutput };

const treeland_output_color_control_v1_listener WQt::TreeLandOutputManager::mColorControlListener = {
    handleResult,  // result
    handleColorTemperature,  // color_temperature
    handleBrightness
};
