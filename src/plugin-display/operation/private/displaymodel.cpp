//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "displaymodel.h"

using namespace dccV25;
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
    , m_monitorModeChanging(true)
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
    //  按照名称排序，显示的时候VGA在前，HDMI在后
    std::sort(m_monitors.begin(), m_monitors.end(), [=](const Monitor *m1, const Monitor *m2){
        return m1->name() > m2->name();
    });
    checkAllSupportFillModes();

    Q_EMIT monitorListChanged();
}

void DisplayModel::monitorRemoved(Monitor *mon)
{
    m_monitors.removeOne(mon);
    checkAllSupportFillModes();

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

void DisplayModel::setTouchscreenList(const TouchscreenInfoList_V2 &touchscreenList)
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

void DisplayModel::setColorTemperatureEnabled(bool enabled)
{
    if (enabled == m_colorTemperatureEnabled)
        return;

    m_colorTemperatureEnabled = enabled;

    Q_EMIT colorTemperatureEnabledChanged(m_colorTemperatureEnabled);
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

void DisplayModel::setCustomColorTempTimePeriod(const QString &timePeriod)
{
    if (m_customColorTempTimePeriod == timePeriod)
        return;

    m_customColorTempTimePeriod = timePeriod;
    Q_EMIT customColorTempTimePeriodChanged(m_customColorTempTimePeriod);
}

void DisplayModel::setRedshiftIsValid(bool redshiftIsValid)
{
    if (m_redshiftIsValid == redshiftIsValid)
        return;

    m_redshiftIsValid = redshiftIsValid;

    Q_EMIT redshiftVaildChanged(redshiftIsValid);
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

void DisplayModel::checkAllSupportFillModes()
{
    for (auto m : monitorList()) {
        if (m->availableFillModes().isEmpty()) {
            m_allSupportFillModes = false;
            return;
        }
    }
    m_allSupportFillModes = true;
}

void DisplayModel::setmodeChanging(bool changing)
{
    m_monitorModeChanging = changing;
}
