// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qwayland-treeland-output-manager-unstable-v2.h"

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

class WQt::ColorControl : public QWaylandClientExtensionTemplate<WQt::ColorControl>, public QtWayland::treeland_output_picture_control_v2
{
    Q_OBJECT

public:
    ColorControl(::treeland_output_picture_control_v2 *obj, QObject *parent = nullptr);
    ~ColorControl() override;

    void setBrightness(double brightness);
    void setColorTemperature(uint32_t temperature);

protected:
    void treeland_output_picture_control_v2_result(uint32_t result) override;
    void treeland_output_picture_control_v2_color_temperature(uint32_t temperature) override;
    void treeland_output_picture_control_v2_brightness(int32_t brightness) override;

Q_SIGNALS:
    // v2 commit_result enum: 0 = success, 1 = failed, 2 = unsupported,
    // 3 = invalid_output. Out-of-range set_* values never reach a commit:
    // they are rejected at request time with a fatal protocol error that
    // terminates the connection (see the clamps in setBrightness and
    // setColorTemperature).
    // (v1 used uint success with 1 = success / 0 = failure — the polarity is
    // inverted in v2, so do not treat a non-zero value as success.)
    void result(uint32_t result);
    void colorTemperatureChanged(uint32_t temperature);
    void brightnessChanged(double brightness);
};

class WQt::TreeLandOutputManager : public QWaylandClientExtensionTemplate<WQt::TreeLandOutputManager>, public QtWayland::treeland_output_manager_v2
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

public:
    TreeLandOutputManager(QObject *parent = nullptr);
    ~TreeLandOutputManager() override;

    void setPrimaryOutput(struct wl_output *output);
    WQt::ColorControl *getPictureControl(struct wl_output *output);

    QString mPrimaryOutput;

protected:
    void treeland_output_manager_v2_primary_output(struct wl_output *output) override;
    void treeland_output_manager_v2_primary_output_failed(uint32_t reason) override;

Q_SIGNALS:
    void primaryOutputChanged(const QString &);
};
