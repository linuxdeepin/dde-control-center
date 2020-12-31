/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "displaymodel.h"

using namespace dcc;
using namespace dcc::display;

const double DoubleZero = 0.000001;

bool contains(const QList<Resolution> &container, const Resolution &item)
{
    for (auto r : container)
        if (r.width() == item.width() && r.height() == item.height())
            return true;

    return false;
}

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
    , m_screenHeight(0)
    , m_screenWidth(0)
    , m_uiScale(1)
    , m_minimumBrightnessScale(0.0)
    , m_redshiftIsValid(false)
    , m_allowEnableMultiScaleRatio(false)
    , m_resolutionRefreshEnable(true)
    , m_brightnessEnable(true)
{
}

double DisplayModel::monitorScale(Monitor *moni)
{
    qDebug() << "ui scale : " << m_uiScale << "\tmonitor scale:" << moni->scale();
    return moni->scale() < 1.0 ? m_uiScale : moni->scale();
}

Monitor *DisplayModel::primaryMonitor() const
{
    for (auto mon : m_monitors)
        if (mon->name() == m_primary)
            return mon;

    return nullptr;
}

void DisplayModel::setScreenHeight(const int h)
{
    if (m_screenHeight != h) {
        m_screenHeight = h;
        Q_EMIT screenHeightChanged(m_screenHeight);
    }
}

void DisplayModel::setScreenWidth(const int w)
{
    if (m_screenWidth != w) {
        m_screenWidth = w;
        Q_EMIT screenWidthChanged(m_screenWidth);
    }
}

void DisplayModel::setDisplayMode(const int mode)
{
    if (m_mode != mode && mode >= 0 && mode < 5) {
        m_mode = mode;
        Q_EMIT displayModeChanged(m_mode);
    }
}

void DisplayModel::setUIScale(const double scale)
{
    if (fabs(m_uiScale - scale) > DoubleZero) {
        m_uiScale = scale;
        Q_EMIT uiScaleChanged(m_uiScale);
    }
}

void DisplayModel::setMinimumBrightnessScale(const double scale)
{
    if (fabs(m_minimumBrightnessScale - scale) > DoubleZero) {
        m_minimumBrightnessScale = scale;
        Q_EMIT minimumBrightnessScaleChanged(m_minimumBrightnessScale);
    }
}

void DisplayModel::setPrimary(const QString &primary)
{
    if (m_primary != primary) {
        m_primary = primary;
        Q_EMIT primaryScreenChanged(m_primary);
    }
}

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    Q_EMIT monitorListChanged();
}

void DisplayModel::monitorRemoved(Monitor *mon)
{
    m_monitors.removeOne(mon);

    Q_EMIT monitorListChanged();
}

void DisplayModel::setAutoLightAdjustIsValid(bool ala)
{
    if (m_AutoLightAdjustIsValid == ala)
        return;
    m_AutoLightAdjustIsValid = ala;
    Q_EMIT autoLightAdjustVaildChanged(ala);
}

void DisplayModel::setBrightnessMap(const BrightnessMap &brightnessMap)
{
    if (brightnessMap == m_brightnessMap)
        return;

    m_brightnessMap = brightnessMap;
}

void DisplayModel::setTouchscreenList(const TouchscreenInfoList &touchscreenList)
{
    if (touchscreenList == m_touchscreenList)
        return;

    m_touchscreenList = touchscreenList;

    Q_EMIT touchscreenListChanged();
}

void DisplayModel::setTouchMap(const TouchscreenMap &touchMap)
{
    if (touchMap == m_touchMap)
        return;

    m_touchMap = touchMap;

    Q_EMIT touchscreenMapChanged();
}

void DisplayModel::setAutoLightAdjust(bool ala)
{
    if (ala == m_isAutoLightAdjust)
        return;

    m_isAutoLightAdjust = ala;

    Q_EMIT autoLightAdjustSettingChanged(m_isAutoLightAdjust);
}

bool DisplayModel::redshiftIsValid() const
{
    return m_redshiftIsValid;
}

void DisplayModel::setAdjustCCTmode(int mode)
{
    if (m_adjustCCTMode == mode)
        return;

    m_adjustCCTMode = mode;

    Q_EMIT adjustCCTmodeChanged(mode);
}

void DisplayModel::setColorTemperature(int value)
{
    if (m_colorTemperature == value)
        return;

    m_colorTemperature = value;

    Q_EMIT colorTemperatureChanged(value);
}

void DisplayModel::setRedshiftIsValid(bool redshiftIsValid)
{
    if (m_redshiftIsValid == redshiftIsValid)
        return;

    m_redshiftIsValid = redshiftIsValid;

    Q_EMIT colorTemperatureChanged(redshiftIsValid);
}

void DisplayModel::setAllowEnableMultiScaleRatio(bool allowEnableMultiScaleRatio)
{
    if (m_allowEnableMultiScaleRatio == allowEnableMultiScaleRatio)
        return;

    m_allowEnableMultiScaleRatio = allowEnableMultiScaleRatio;
}

void DisplayModel::setRefreshRateEnable(bool isEnable)
{
    m_RefreshRateEnable = isEnable;
}

void DisplayModel::setmaxBacklightBrightness(const uint value)
{
    if (m_maxBacklightBrightness != value && value < 100) {
        m_maxBacklightBrightness = value;
        Q_EMIT maxBacklightBrightnessChanged(value);
    }
}

void DisplayModel::setResolutionRefreshEnable(const bool enable)
{
    if (m_resolutionRefreshEnable != enable) {
        m_resolutionRefreshEnable = enable;
        Q_EMIT resolutionRefreshEnableChanged(m_resolutionRefreshEnable);
    }
}

void DisplayModel::setBrightnessEnable(const bool enable)
{
    if (m_brightnessEnable != enable) {
        m_brightnessEnable = enable;
        Q_EMIT brightnessEnableChanged(m_brightnessEnable);
    }
}
