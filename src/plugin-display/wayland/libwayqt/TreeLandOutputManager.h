// SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <wayland-client-protocol.h>

#include <QMap>
#include <QObject>
#include <QRect>
#include <QString>

struct wl_buffer;
struct wl_output;
struct treeland_output_manager_v1;
struct treeland_output_manager_v1_listener;
struct treeland_output_color_control_v1;
struct treeland_output_color_control_v1_listener;

namespace WQt {
class TreeLandOutputManager;
}

class WQt::TreeLandOutputManager : public QObject
{
    Q_OBJECT;

public:
    TreeLandOutputManager(treeland_output_manager_v1 *);
    ~TreeLandOutputManager();

    /** Set the primary output */
    void setPrimaryOutput(const char *);

    QString mPrimaryOutput;

    treeland_output_color_control_v1 * getColorControl(struct wl_output *output);
    void setBrightness(treeland_output_color_control_v1 * treeland_output_color_control_v1,const double brightness);
    void destroyColorControl(treeland_output_color_control_v1 *treeland_output_color_control_v1);
private:
    static void handlePrimaryOutput(void *data,
                                    struct treeland_output_manager_v1 *treeland_output_manager_v1,
                                    const char *output_name);
    static void handleResult(void *data,
                             struct treeland_output_color_control_v1 *treeland_output_color_control_v1,
                             uint32_t success);
    static void handleColorTemperature(void *data,
                                       struct treeland_output_color_control_v1 *treeland_output_color_control_v1,
                                       uint32_t temperature);
    static void handleBrightness(void *data,
                                 struct treeland_output_color_control_v1 *treeland_output_color_control_v1,
                                 wl_fixed_t brightness);

    static const treeland_output_manager_v1_listener mListener;
    static const treeland_output_color_control_v1_listener mColorControlListener;

    treeland_output_manager_v1 *mObj;

Q_SIGNALS:
    void primaryOutputChanged(const char *);
    void brightnessChanged(const treeland_output_color_control_v1 *,double brightness);
    void colorTemperatureChanged(const treeland_output_color_control_v1 *,uint temperature);
};
