/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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
    void requestSetMainwindowRect(dcc::display::Monitor *monitor);

private:
    void initModeList();
    void initPrimaryList();
    void initSecondaryScreenDialog();

private Q_SLOTS:
    void onGatherWindows(const QPoint cursor);
    void onMonitorPress(dcc::display::Monitor *monitor);
    void onMonitorRelease(dcc::display::Monitor *monitor);
    void onRequestSetMonitorPosition(QHash<dcc::display::Monitor *, QPair<int, int>> monitorPosition);
    void onRequestShowSecondaryScreen();

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
    SecondaryScreenDialog *m_dlg;

    dcc::display::DisplayModel *m_model;

    QList<SecondaryScreenDialog *> m_secondaryScreenDlgList;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // MULTISCREENWIDGET_H
