/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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
#include <QGSettings>

#include <com_deepin_daemon_display.h>

using DisplayInter = com::deepin::daemon::Display;

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

public:
    void initGSettings();

public slots:
    void saveChanges();
    void discardChanges();
    void mergeScreens();
    void splitScreens();
//    void switchCustom(const bool deleteConfig = true);
    void createConfig();
    void switchConfig(const QString &config);
    void deleteConfig(const QString &config);
    void modifyConfigName(const QString &oldName, const QString &newName);
    void switchMode(const int mode, const QString &name = QString());
    void setPrimary(const int index);
    void setMonitorEnable(Monitor *mon, const bool enabled);
#ifndef DCC_DISABLE_ROTATE
    void setMonitorRotateAll(const quint16 rotate);
    void setMonitorRotate(Monitor *mon, const quint16 rotate);
#endif
    void setMonitorResolution(Monitor *mon, const int mode);
    void setMonitorBrightness(Monitor *mon, const double brightness);
    void setMonitorPosition(Monitor *mon, const int x, const int y);
    void setUiScale(const double value);

private slots:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onModifyConfigNameFinished(QDBusPendingCallWatcher *w);

    void onXSettingsChanged(const QString &key);

private:
//    void loadRotations(Monitor * const mon);
//    void loadRotationsFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
//    void loadModes(Monitor * const mon);
//    void loadModesFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
    void createConfigFinshed(QDBusPendingCallWatcher *w);
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);
    void updateMonitorBrightness(const QString &monName, const double brightness);

private:
    DisplayModel *m_model;
    QGSettings *m_xsettings;
    DisplayInter m_displayInter;
    QMap<Monitor *, MonitorInter *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWORKER_H
