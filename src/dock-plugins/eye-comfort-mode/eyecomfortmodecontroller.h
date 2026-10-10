// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef EYE_COMFORT_MODE_CONTROLLER_H
#define EYE_COMFORT_MODE_CONTROLLER_H

#include "appearance1interface.h"

#include <DSingleton>

#include <QObject>
#include <QDBusInterface>

using DBusTheme = org::deepin::dde::Appearance1;

class EyeComfortModeController : public QObject, public Dtk::Core::DSingleton<EyeComfortModeController>
{
    friend Dtk::Core::DSingleton<EyeComfortModeController>;

    Q_OBJECT
public:
    void enable(bool enable);
    bool isEyeComfortModeEnabled() const { return m_eyeComfortModeEnabled; }
    void toggle() { enable(!isEyeComfortModeEnabled()); }

    bool supportColorTemperature() const { return m_supportColorTemperature; }
    QString gtkTheme() const;
    QString globalTheme() const;

Q_SIGNALS:
    void eyeComfortModeChanged(bool enabled);
    void supportColorTemperatureChanged(bool support);
    void globalThemeChanged(const QString &value);

public slots:
    void setGtkTheme(const QString &value);
    // 适配v23接口
    void setGlobalTheme(const QString &value);

private:
    EyeComfortModeController();

    ~EyeComfortModeController() {}

private slots:
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);

private:
    QDBusInterface *m_displayInter;
    DBusTheme *m_themeInter;
    bool m_eyeComfortModeEnabled;
    bool m_supportColorTemperature;
};

#endif //EYE_COMFORT_MODE_CONTROLLER_H
