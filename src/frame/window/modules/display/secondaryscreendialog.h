/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef SECONDARYSCREENDIALOG_H
#define SECONDARYSCREENDIALOG_H

#include "interface/namespace.h"

#include <DAbstractDialog>

class Resolution;

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace display {
class DisplayModel;
class Monitor;
class MonitorControlWidget;
} // namespace display

} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {
class ResolutionWidget;
class RefreshRateWidget;
class RotateWidget;

class SecondaryScreenDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    SecondaryScreenDialog(QWidget *parent = nullptr);

public:
    void setModel(dcc::display::DisplayModel *model, dcc::display::Monitor *monitor);
    void resetDialog();

protected:
    void keyPressEvent(QKeyEvent *event) override;

Q_SIGNALS:
    void requestRecognize();
    void requestSetMonitorBrightness(dcc::display::Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);
    void requestSetResolution(dcc::display::Monitor *monitor, const int mode);
    void requestSetRotate(dcc::display::Monitor *monitor, const int rotate);
    void requestGatherWindows(const QPoint cursor);
    void requestGatherEnabled(const bool enable);

private:
    QVBoxLayout *m_contentLayout;
    dcc::display::MonitorControlWidget *m_monitorControlWidget;
    ResolutionWidget *m_resolutionWidget;
    RefreshRateWidget *m_refreshRateWidget;
    RotateWidget *m_rotateWidget;

    dcc::display::DisplayModel *m_model;
    dcc::display::Monitor *m_monitor;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // SECONDARYSCREENDIALOG_H
