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

#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include "monitor.h"

#include <QObject>

#include <com_deepin_daemon_display.h>
#include <com_deepin_daemon_appearance.h>
#include <com_deepin_daemon_power.h>

#include <QGSettings>

using DisplayInter = com::deepin::daemon::Display;
using AppearanceInter = com::deepin::daemon::Appearance;
using PowerInter = com::deepin::daemon::Power;

namespace dcc {

namespace display {

class DisplayModel;
class DisplayWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWorker(DisplayModel *model, QObject *parent = 0, bool isSync = false);
    ~DisplayWorker();

    void active();

public Q_SLOTS:
    void saveChanges();
    void switchMode(const int mode, const QString &name);
    void setPrimary(const QString &name);
    void setMonitorEnable(Monitor *monitor, const bool enable);
    void applyChanges();
    void setColorTemperature(int value);
    void SetMethodAdjustCCT(int mode);
#ifndef DCC_DISABLE_ROTATE
    void setMonitorRotate(Monitor *mon, const quint16 rotate);
#endif
    void setMonitorResolution(Monitor *mon, const int mode);
    void setMonitorBrightness(Monitor *mon, const double brightness);
    void setMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition);
    void setUiScale(const double value);
    void setIndividualScaling(Monitor *m, const double scaling);
    void setNightMode(const bool nightmode);
    void setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID);
    void setMonitorResolutionBySize(Monitor *mon, const int width, const int height);
    void setAmbientLightAdjustBrightness(bool);

private Q_SLOTS:
    void onGSettingsChanged(const QString &key);
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onGetScaleFinished(QDBusPendingCallWatcher *w);
    void onGetScreenScalesFinished(QDBusPendingCallWatcher *w);

private:
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);

Q_SIGNALS:
    void requestUpdateModeList();

private:
    DisplayModel *m_model;
    DisplayInter m_displayInter;
    QDBusInterface *m_displayDBusInter;
    QGSettings *m_dccSettings;
    AppearanceInter *m_appearanceInter;
    QMap<Monitor *, MonitorInter *> m_monitors;
    double m_currentScale;
    bool m_updateScale;
    QTimer *m_timer;

    PowerInter *m_powerInter;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWORKER_H
