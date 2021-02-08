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

#include "modules/display/displaymodel.h"
#include "modules/display/monitorcontrolwidget.h"
#include "modules/display/monitorindicator.h"
#include "scalingwidget.h"
#include "brightnesswidget.h"
#include "secondaryscreendialog.h"
#include "widgets/settingsgroup.h"
#include "widgets/comboxwidget.h"
#include "../../mainwindow.h"

class Resolution;

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace display {
class DisplayModel;
class Monitor;
class MonitorControlWidget;
class MonitorIndicator;
} // namespace display

namespace widgets {
class SettingsGroup;
class ComboxWidget;
} // namespace widgets
} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

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
    void requestSetMonitorPosition(dcc::display::Monitor *monitor, const int x, const int y);
    void requestSetPrimary(const QString &name);
    void requestSetColorTemperature(const int value);
    void requestSetMonitorBrightness(dcc::display::Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);
    void requestSetMethodAdjustCCT(const int mode);
    void requestUiScaleChange(const double scale);
    void requestIndividualScaling(dcc::display::Monitor *monitor, const double scale);
    void requestSetResolution(dcc::display::Monitor *monitor, const int mode);
    void requestSetRotate(dcc::display::Monitor *monitor, const int rotate);
    void requestEnalbeMonitor(dcc::display::Monitor *monitor, const bool enable);
    void requestGatherEnabled(const bool enable);

private:
    void initModeList();
    void initPrimaryList();
    void initSecondaryScreenDialog();

private Q_SLOTS:
    void onGatherWindows(const QPoint cursor);
    void onMonitorPress(dcc::display::Monitor *monitor);
    void onMonitorRelease(dcc::display::Monitor *monitor);

private:
    QVBoxLayout *m_contentLayout;
    dcc::display::MonitorControlWidget *m_monitorControlWidget;
    dcc::display::MonitorIndicator *m_fullIndication;
    TitleLabel *m_multiSettingLabel;
    dcc::widgets::SettingsGroup *m_modeSettingsGroup;
    dcc::widgets::ComboxWidget *m_modeComboxWidget;
    dcc::widgets::SettingsGroup *m_primarySettingsGroup;
    dcc::widgets::ComboxWidget *m_primaryComboxWidget;
    BrightnessWidget *m_brightnessWidget;
    ScalingWidget *m_scalingWidget;
    ResolutionWidget *m_resolutionWidget;
    RefreshRateWidget *m_refreshRateWidget;
    RotateWidget *m_rotateWidget;

    dcc::display::DisplayModel *m_model;

    QList<SecondaryScreenDialog *> m_secondaryScreenDlgList;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // MULTISCREENWIDGET_H
