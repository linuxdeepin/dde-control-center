// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "brightnessmodel.h"
#include "brightness-constants.h"

const double DoubleZero = 0.000001;

BrightnessModel::BrightnessModel(QObject* parent)
    : QObject(parent)
    , m_displayMode(Dock::Brightness::DisplayMode_Mirror)
    , m_minimumBrightnessScale(0.1)
{
}

BrightnessModel::~BrightnessModel()
{
}

void BrightnessModel::setDisplayMode(const int mode)
{
    qCInfo(BRIGHTNESS) << "Set display mode: " << mode;
    if (m_displayMode != mode && mode >= 0 && mode < 5) {
        m_displayMode = mode;
        Q_EMIT displayModeChanged(m_displayMode);
    }
}

void BrightnessModel::setMinimumBrightnessScale(const double scale)
{
    qCInfo(BRIGHTNESS) << "Set minimum brightness scale:" << scale;
    if (fabs(m_minimumBrightnessScale - scale) > DoubleZero) {
        m_minimumBrightnessScale = scale;
        Q_EMIT minBrightnessChanged(m_minimumBrightnessScale);
    }
}

void BrightnessModel::setPrimary(const QString& primary)
{
    qCInfo(BRIGHTNESS) << "Set primary: " << primary;
    if (m_primaryMonitorName != primary) {
        m_primaryMonitorName = primary;
        for (auto mon : m_monitors)
            mon->setPrimary(m_primaryMonitorName);

        Q_EMIT primaryScreenChanged(m_primaryMonitorName);
    }
}

void BrightnessModel::setBuildinMonitorName(const QString& monitorName)
{
    qCInfo(BRIGHTNESS) << "Set buildin monitor name:" << monitorName;
    m_buildinMonitorName = monitorName;
    for (const auto& monitor : m_monitors) {
        monitor->setBuildin(monitor->name() == m_buildinMonitorName);
    }
}

void BrightnessModel::monitorAdded(Monitor* mon)
{
    qCInfo(BRIGHTNESS) << "Monitor added:" << mon->name();
    m_monitors.append(mon);
    connect(mon, &Monitor::enableChanged, this, &BrightnessModel::enabledMonitorListChanged);
    connect(mon, &Monitor::brightnessChanged, this, &BrightnessModel::monitorBrightnessChanged);
    // 按照名称排序
    std::sort(m_monitors.begin(), m_monitors.end(), [=](const Monitor* m1, const Monitor* m2) {
        return m1->name() < m2->name();
    });
    Q_EMIT enabledMonitorListChanged();
}

void BrightnessModel::monitorRemoved(Monitor* mon)
{
    qCInfo(BRIGHTNESS) << "Monitor removed:" << mon->name();
    mon->disconnect(this);
    m_monitors.removeOne(mon);
    Q_EMIT enabledMonitorListChanged();
}

Monitor* BrightnessModel::primaryMonitor() const
{
    for (const auto& monitor : m_monitors) {
        if (monitor->isPrimary())
            return monitor;
    }

    return nullptr;
}

/**
 * @brief
 * 复制模式内建屏幕优先，扩展模式主屏优先，不能调节亮度的屏幕放在后面
 *
 * @return const QList<Monitor*>
 */
QList<Monitor*> BrightnessModel::enabledMonitors() const
{
    QList<Monitor*> monitors;
    QList<Monitor*> canNotAdjustBrightness;
    for (const auto& monitor : m_monitors) {
        if (monitor->isEnabled()) {
            if (monitor->canBrightness()) {
                if ((Dock::Brightness::DisplayMode_Externd == m_displayMode && monitor->isPrimary())
                    || (Dock::Brightness::DisplayMode_Mirror && monitor->isBuildin())) {
                    monitors.insert(0, monitor);
                } else {
                    monitors.append(monitor);
                }
            } else {
                canNotAdjustBrightness.append(monitor);
            }
        }
    }
    monitors.append(canNotAdjustBrightness);

    return monitors;
}

Monitor* BrightnessModel::monitor(const QString& monitorName) const
{
    for (const auto& monitor : m_monitors) {
        if (monitor->name() == monitorName)
            return monitor;
    }

    return nullptr;
}

void BrightnessModel::setBrightnessMap(const BrightnessMap& brightnessMap)
{
    qCInfo(BRIGHTNESS) << "Set brightness map:" << brightnessMap;
    if (brightnessMap == m_brightnessMap)
        return;

    m_brightnessMap = brightnessMap;
}

uint BrightnessModel::maxBrightness() const
{
    static const double BRIGHTNESS_MAX_SCALE = 100.0;
    return BRIGHTNESS_MAX_SCALE;
}

uint BrightnessModel::minBrightness() const
{
    const int value = maxBrightness() * m_minimumBrightnessScale;
    return value == 0 ? 1 : value;
}
