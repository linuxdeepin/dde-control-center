// SPDX-FileCopyrightText: 2011 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QScreen>

#include "display1monitorinterface.h"

using MonitorInter = org::deepin::dde::display1::Monitor;


class BrightnessController;
class Monitor : public QObject
{
    Q_OBJECT
    friend class BrightnessController;

public:
    enum RotateMode {
        Normal,
        Gravity,
        Rotate
    };

public:
    explicit Monitor(QObject *parent = 0);

    inline bool isPrimary() const { return m_primary == m_name; }
    inline double brightness() const { return m_brightness; }
    inline const QString name() const { return m_name; }
    inline bool canBrightness() const { return m_canBrightness; }
    inline const QString path() const { return m_path; }
    inline bool isEnabled() const { return m_enable; }
    inline bool isBuildin() const { return m_buildin; };

Q_SIGNALS:
    void brightnessChanged(const double brightness) const;
    void enableChanged(bool enable) const;

public:
    static bool isSameResolution(const Resolution &r1, const Resolution &r2);
    static bool isSameRatefresh(const Resolution &r1, const Resolution &r2);
    bool hasResolution(const Resolution &r);
    bool hasResolutionAndRate(const Resolution &r);
    bool hasRatefresh(const double r);
    QScreen *getQScreen();
    void setPrimary(const QString &primaryName);
    void setBuildin(bool buildin);

private Q_SLOTS:
    void setBrightness(const double brightness);
    void setName(const QString &name);
    void setCanBrightness(bool canBrightness);
    void setPath(const QString &path);
    void setMonitorEnable(bool enable);

private:
    double m_brightness;
    QString m_name;
    QString m_path;
    QString m_primary;
    bool m_enable;
    bool m_canBrightness;
    bool m_buildin;
};

#endif // MONITOR_H
