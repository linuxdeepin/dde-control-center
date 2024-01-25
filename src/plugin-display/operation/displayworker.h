//SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include "interface/namespace.h"
#include "displaydbusproxy.h"
#include "monitor.h"

#include <dtkcore_global.h>

#include <QObject>
#include <QTimer>
#include <QProcess>

#define GAMMA_SUPPORT false
/*
 * Disable gamma support for treeland now
 * We can't keep GammaTable when dde-control-center close
 * We need write a daemon in future
 */

DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE

namespace WQt {
    class Output;
    class Registry;
    class OutputHead;
}

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

    void backupConfig();
    void clearBackup();
    void resetBackup();

private Q_SLOTS:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onGetScaleFinished(QDBusPendingCallWatcher *w);
    void onGetScreenScalesFinished(QDBusPendingCallWatcher *w);

    // for wlroots-based compositors
    void onWlMonitorListChanged();

private:
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);

    // for wlroots-based compositors
    void wlMonitorAdded(WQt::OutputHead *head);
    void wlMonitorRemoved(WQt::OutputHead *head);

Q_SIGNALS:
    void requestUpdateModeList();

private:
    DisplayModel *m_model;
    DisplayDBusProxy *m_displayInter;
    QMap<Monitor *, MonitorDBusProxy *> m_monitors;

    // for wlroots-based compositors
    WQt::Registry *m_reg { nullptr };
    QMap<Monitor *, WQt::OutputHead *> m_wl_monitors;
#if GAMMA_SUPPORT
    QMap<Monitor *, DFL::GammaEffects *> *m_wl_gammaEffects;
    QMap<Monitor *, DFL::GammaEffectsConfig *> *m_wl_gammaConfig;
#endif

    double m_currentScale;
    bool m_updateScale;
    QTimer *m_timer;
    DTK_CORE_NAMESPACE::DConfig *m_dconfig;
    QString m_displayConfig;
};
}

#endif // DISPLAYWORKER_H
