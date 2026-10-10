// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BRIGHTNESSMODEL_H
#define BRIGHTNESSMODEL_H

#include "monitor.h"

#include <QObject>

#include <DSingleton>

#include <types/brightnessmap.h>

class BrightnessModel : public QObject, public Dtk::Core::DSingleton<BrightnessModel> {
    Q_OBJECT

    friend class Dtk::Core::DSingleton<BrightnessModel>;

public:
    void setDisplayMode(const int mode);
    inline int displayMode() const { return m_displayMode; }

    void setPrimary(const QString& primary);
    inline const QString& primary() const { return m_primaryMonitorName; }

    void setBuildinMonitorName(const QString& monitorName);
    inline const QString& buildinMonitor() const { return m_buildinMonitorName; }

    void monitorAdded(Monitor* mon);
    void monitorRemoved(Monitor* mon);
    inline const QList<Monitor*> monitorList() const { return m_monitors; }
    QList<Monitor*> enabledMonitors() const;
    Monitor* primaryMonitor() const;
    Monitor* monitor(const QString& monitorName) const;

    void setBrightnessMap(const BrightnessMap& brightnessMap);
    inline BrightnessMap brightnessMap() const { return m_brightnessMap; }

    uint maxBrightness() const;
    void setMinimumBrightnessScale(const double scale);
    inline double minimumBrightnessScale() const { return m_minimumBrightnessScale; }
    uint minBrightness() const;

Q_SIGNALS:
    void displayModeChanged(const int mode) const;
    void primaryScreenChanged(const QString& primary) const;
    void monitorListChanged() const;
    void enabledMonitorListChanged() const;
    void monitorBrightnessChanged() const;
    void minBrightnessChanged(const double) const;

private:
    explicit BrightnessModel(QObject* parent = nullptr);
    ~BrightnessModel();

private:
    int m_displayMode;
    double m_minimumBrightnessScale;
    QString m_primaryMonitorName;
    QString m_buildinMonitorName;
    QList<Monitor*> m_monitors;
    BrightnessMap m_brightnessMap;
};

#endif
