//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include "interface/namespace.h"
#include "displaydbusproxy.h"
#include "machine.h"
#include "machinedbusproxy.h"
#include "monitor.h"

#include <dtkcore_global.h>

#include <QObject>
#include <QTimer>
#include <QProcess>

DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE

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
    void setDeviceSharingSwitch(const bool enable);
    void setCurrentMachineConnect(Machine *mac);
    void setCurrentRequestDeviceSharing(Machine *mac);
    void setCurrentMachineDisconnect(Machine *mac);
    void setCurrentStopDeviceSharing(Machine *mac);
    void setOpenSharedDevices(bool on);
    void setOpenSharedClipboard(bool on);
    void setFilesStoragePath(const QString& path);
    void setFlowDirection(Machine *mac, const int& dir);

    void backupConfig();
    void clearBackup();
    void resetBackup();

private Q_SLOTS:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onGetScaleFinished(QDBusPendingCallWatcher *w);
    void onGetScreenScalesFinished(QDBusPendingCallWatcher *w);
    void onMachinesChanged(const QList<QDBusObjectPath> &machines);
    void onHistoryDevChanged(const QList<QString> &machines);

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
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
    QString m_displayConfig;
};
}

#endif // DISPLAYWORKER_H
