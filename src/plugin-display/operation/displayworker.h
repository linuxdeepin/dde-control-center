/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             Tinalu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             Tinalu Shao <shaotianlu@uniontech.com>
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

#include "interface/namespace.h"
#include "displaydbusproxy.h"
#include "machine.h"
#include "machinedbusproxy.h"
#include "monitor.h"


#include <QObject>
#include <QTimer>
#include <QProcess>

namespace DCC_NAMESPACE {
class DisplayModel;
class DisplayWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWorker(DisplayModel *model, QObject *parent = nullptr, bool isSync = false);
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
    void setCurrentFillMode(Monitor *mon, const QString fillMode);
    // Cooperation Machine
    void setCooperationEnable(const bool enable);
    void setCurrentMachinePair(Machine *mac);
    void setCurrentMachineDisconnect(Machine *mac);

private Q_SLOTS:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onGetScaleFinished(QDBusPendingCallWatcher *w);
    void onGetScreenScalesFinished(QDBusPendingCallWatcher *w);
    void onMachinesChanged(const QList<QDBusObjectPath> &machines);

private:
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);

    void machinesAdded(const QString &path);
    void machinesRemoved(const QString &path);

Q_SIGNALS:
    void requestUpdateModeList();

private:
    DisplayModel *m_model;
    DisplayDBusProxy *m_displayInter;
    QMap<Monitor *, MonitorDBusProxy *> m_monitors;
    QMap<Machine *, MachineDBusProxy *> m_machines;
    double m_currentScale;
    bool m_updateScale;
    QTimer *m_timer;
};
}

#endif // DISPLAYWORKER_H
