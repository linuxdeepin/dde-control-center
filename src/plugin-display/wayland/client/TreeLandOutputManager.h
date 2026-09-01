// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-treeland-output-manager-v1.h"

#include <QMap>
#include <QObject>
#include <QRect>
#include <QString>
#include <QtWaylandClient/QWaylandClientExtension>

struct wl_buffer;
struct wl_output;

namespace WQt {
class TreeLandOutputManager;
class ColorControl;
} // namespace WQt

class WQt::ColorControl : public QWaylandClientExtensionTemplate<WQt::ColorControl>, public QtWayland::treeland_output_color_control_v1
{
    Q_OBJECT

public:
    ColorControl(::treeland_output_color_control_v1 *obj, QObject *parent = nullptr);
    ~ColorControl() override;

    void setBrightness(double brightness);
    void setColorTemperature(uint32_t temperature);

protected:
    void treeland_output_color_control_v1_result(uint32_t success) override;
    void treeland_output_color_control_v1_color_temperature(uint32_t temperature) override;
    void treeland_output_color_control_v1_brightness(int32_t brightness) override;

Q_SIGNALS:
    void result(uint32_t success);
    void colorTemperatureChanged(uint32_t temperature);
    void brightnessChanged(double brightness);
};

class WQt::TreeLandOutputManager : public QWaylandClientExtensionTemplate<WQt::TreeLandOutputManager>, public QtWayland::treeland_output_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    TreeLandOutputManager(QObject *parent = nullptr);
    ~TreeLandOutputManager() override;

    void setPrimaryOutput(const char *);
    WQt::ColorControl *getColorControl(struct wl_output *output);

    QString mPrimaryOutput;

protected:
    void treeland_output_manager_v1_primary_output(const QString &output_name) override;

Q_SIGNALS:
    void primaryOutputChanged(const QString &);
};
