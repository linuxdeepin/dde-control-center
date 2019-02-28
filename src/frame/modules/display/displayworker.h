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

#include <QGSettings>

using DisplayInter = com::deepin::daemon::Display;
using AppearanceInter = com::deepin::daemon::Appearance;

#define CUSTOM_MODE     0
#define MERGE_MODE      1
#define EXTEND_MODE     2
#define SINGLE_MODE     3

namespace dcc {

namespace display {

class DisplayModel;
class DisplayWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWorker(DisplayModel *model, QObject *parent = 0);
    ~DisplayWorker();

    void active();

public Q_SLOTS:
    void saveChanges();
    void discardChanges();
    void mergeScreens();
    void splitScreens();
    void duplicateMode();
    void extendMode();
    void onlyMonitor(const QString &monName);
    void createConfig(const QString &config);
    void switchConfig(const QString &config);
    void deleteConfig(const QString &config);
    void modifyConfigName(const QString &oldName, const QString &newName);
    void switchMode(const int mode, const QString &name = QString());
    void setPrimary(const int index);
    void setMonitorEnable(Monitor *mon, const bool enabled);
    void applyChanges();
#ifndef DCC_DISABLE_ROTATE
    void setMonitorRotateAll(const quint16 rotate);
    void setMonitorRotate(Monitor *mon, const quint16 rotate);
#endif
    void setMonitorResolution(Monitor *mon, const int mode);
    void setMonitorBrightness(Monitor *mon, const double brightness);
    void setMonitorPosition(Monitor *mon, const int x, const int y);
    void setUiScale(const double value);
    void setIndividualScaling(Monitor *m, const double scaling);
    void setNightMode(const bool nightmode);
    void updateNightModeStatus();
    void record(); // save mutilScreenConfig
    void restore(); // restore mutilScreenConfig

private Q_SLOTS:
    void onGSettingsChanged(const QString &key);
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onModifyConfigNameFinished(QDBusPendingCallWatcher *w);
    void onGetScaleFinished(QDBusPendingCallWatcher *w);
    void onGetScreenScalesFinished(QDBusPendingCallWatcher *w);
    void onCreateConfigFinshed(QDBusPendingCallWatcher *w);

private:
//    void loadRotations(Monitor * const mon);
//    void loadRotationsFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
//    void loadModes(Monitor * const mon);
//    void loadModesFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);

private:
    DisplayModel *m_model;
    DisplayInter m_displayInter;
    QGSettings *m_dccSettings;
    AppearanceInter *m_appearanceInter;
    QMap<Monitor *, MonitorInter *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWORKER_H
