//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef MULTISCREENWIDGET_H
#define MULTISCREENWIDGET_H

#include "interface/namespace.h"
#include "widgets/titlelabel.h"

class Resolution;

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QSpacerItem;
class QHBoxLayout;
class QVBoxLayout;
class QKeyEvent;
class QTimer;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class SettingsItem;
class Monitor;
class Machine;
class DisplayModel;
class MonitorControlWidget;
class MonitorIndicator;
class RecognizeWidget;
class BrightnessWidget;
class ScalingWidget;
class ResolutionWidget;
class RefreshRateWidget;
class RotateWidget;
class SecondaryScreenDialog;
class CollaborativeLinkWidget;
class MultiScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiScreenWidget(QWidget *parent = nullptr);
    ~MultiScreenWidget();

public:
    void setModel(DisplayModel *model);

Q_SIGNALS:
    void requestRecognize();
    void requestSwitchMode(const int mode, const QString &name = QString());
    void requestSetMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition);
    void requestSetPrimary(const QString &name);
    void requestSetColorTemperature(const int value);
    void requestSetMonitorBrightness(Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);
    void requestSetMethodAdjustCCT(const int mode);
    void requestUiScaleChange(const double scale);
    void requestSetResolution(Monitor *monitor, const int mode);
    void requestSetRotate(Monitor *monitor, const int rotate);
    void requestGatherEnabled(const bool enable);
    void requestSetMainwindowRect(Monitor *monitor, bool isInit);
    void requestSetFillMode(Monitor *monitor, const QString fillMode);
    void requestCurrFillModeChanged(Monitor *monitor, const QString fillMode);

private:
    void initModeList();
    void initPrimaryList();
    void initSecondaryScreenDialog();

private Q_SLOTS:
    void onGatherWindows(const QPoint cursor);
    void onMonitorPress(Monitor *monitor);
    void onMonitorRelease(Monitor *monitor);
    void onRequestSetMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition);
    void onRequestRecognize();
    void onRequestCloseRecognize();
    void onResetSecondaryScreenDlg();
    void onResetFullIndication(const QRect &geometry);
    void onResetSecondaryScreenDlgTimerOut();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVBoxLayout *m_contentLayout;
    MonitorControlWidget *m_monitorControlWidget;
    MonitorIndicator *m_fullIndication;
    TitleLabel *m_multiSettingLabel;
    SettingsItem *m_modeSettingsItem;
    QLabel *m_modeLabel;
    QComboBox *m_modeCombox;
    SettingsItem *m_primarySettingsItem;
    QLabel *m_primaryLabel;
    QComboBox *m_primaryCombox;

    //协同链接
    CollaborativeLinkWidget *m_linkWidget;
    BrightnessWidget *m_brightnessWidget;
    ScalingWidget *m_scalingWidget;
    ResolutionWidget *m_resolutionWidget;
    RefreshRateWidget *m_refreshRateWidget;
    RotateWidget *m_rotateWidget;

    DisplayModel *m_model;

    QList<SecondaryScreenDialog *> m_secondaryScreenDlgList;
    QMap<QString, RecognizeWidget *> m_recognizeWidget;

    bool isReleaseMonitor = false;
    QTimer *m_resetSecondaryScreenDlgTimer;
};
}

#endif // MULTISCREENWIDGET_H
