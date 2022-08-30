// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
QT_END_NAMESPACE

namespace dcc {
namespace display {
class Monitor;
class DisplayModel;
class MonitorControlWidget;
class MonitorIndicator;
class RecognizeWidget;
} // namespace display

namespace widgets {
class SettingsItem;
} // namespace widgets
} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {
class BrightnessWidget;
class ScalingWidget;
class ResolutionWidget;
class RefreshRateWidget;
class RotateWidget;
class SecondaryScreenDialog;

class MultiScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiScreenWidget(QWidget *parent = nullptr);
    ~MultiScreenWidget();

public:
    void setModel(dcc::display::DisplayModel *model);

Q_SIGNALS:
    void requestRecognize();
    void requestSwitchMode(const int mode, const QString &name = QString());
    void requestSetMonitorPosition(QHash<dcc::display::Monitor *, QPair<int, int>> monitorPosition);
    void requestSetPrimary(const QString &name);
    void requestSetColorTemperature(const int value);
    void requestSetMonitorBrightness(dcc::display::Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);
    void requestSetMethodAdjustCCT(const int mode);
    void requestUiScaleChange(const double scale);
    void requestSetResolution(dcc::display::Monitor *monitor, const int mode);
    void requestSetRotate(dcc::display::Monitor *monitor, const int rotate);
    void requestGatherEnabled(const bool enable);
    void requestSetMainwindowRect(dcc::display::Monitor *monitor, bool isInit);
    void requestSetFillMode(dcc::display::Monitor *monitor, const QString fillMode);
    void requestCurrFillModeChanged(dcc::display::Monitor *monitor, const QString fillMode);

private:
    void initModeList();
    void initPrimaryList();
    void initSecondaryScreenDialog();
    void setModeCurrentIndex(const QString &monitorName);

public Q_SLOTS:
    void onMainwindowStateChanged(int type);

private Q_SLOTS:
    void onGatherWindows(const QPoint cursor);
    void onMonitorPress(dcc::display::Monitor *monitor);
    void onMonitorRelease(dcc::display::Monitor *monitor);
    void onRequestSetMonitorPosition(QHash<dcc::display::Monitor *, QPair<int, int>> monitorPosition);
    void onRequestRecognize();
    void onRequestCloseRecognize();
    void onResetSecondaryScreenDlg();
    void onResetFullIndication(const QRect &geometry);
    void onResetSecondaryScreenDlgTimerOut();
    void onGSettingsChanged(const QString & gsettingsName, const QString &setting);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVBoxLayout *m_contentLayout;
    dcc::display::MonitorControlWidget *m_monitorControlWidget;
    dcc::display::MonitorIndicator *m_fullIndication;
    TitleLabel *m_multiSettingLabel;
    dcc::widgets::SettingsItem *m_modeSettingsItem;
    QLabel *m_modeLabel;
    QComboBox *m_modeCombox;
    dcc::widgets::SettingsItem *m_primarySettingsItem;
    QLabel *m_primaryLabel;
    QComboBox *m_primaryCombox;
    QSpacerItem *m_brightnessSpacerItem;
    BrightnessWidget *m_brightnessWidget;
    ScalingWidget *m_scalingWidget;
    ResolutionWidget *m_resolutionWidget;
    RefreshRateWidget *m_refreshRateWidget;
    RotateWidget *m_rotateWidget;

    dcc::display::DisplayModel *m_model;

    QList<SecondaryScreenDialog *> m_secondaryScreenDlgList;
    QMap<QString, dcc::display::RecognizeWidget *> m_recognizeWidget;

    bool isReleaseMonitor = false;
    QTimer *m_resetSecondaryScreenDlgTimer;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // MULTISCREENWIDGET_H
