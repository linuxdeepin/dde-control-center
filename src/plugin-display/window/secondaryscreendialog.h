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

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QKeyEvent;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class ResolutionWidget;
class RefreshRateWidget;
class RotateWidget;
class DisplayModel;
class Monitor;
class MonitorControlWidget;

class SecondaryScreenDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit SecondaryScreenDialog(QWidget *parent = nullptr);
    ~SecondaryScreenDialog();

public:
    void setModel(DisplayModel *model, Monitor *monitor);
    void resetDialog();

protected:
    void keyPressEvent(QKeyEvent *event) override;

Q_SIGNALS:
    void requestRecognize();
    void requestSetMonitorBrightness(Monitor *monitor, const double brightness);
    void requestAmbientLightAdjustBrightness(const bool able);
    void requestSetResolution(Monitor *monitor, const int mode);
    void requestSetFillMode(Monitor *monitor, const QString fillMode);
    void requestSetRotate(Monitor *monitor, const int rotate);
    void requestGatherWindows(const QPoint cursor);
    void requestGatherEnabled(const bool enable);
    void requestCloseRecognize();
    void requestCurrFillModeChanged(Monitor *monitor, const QString fillMode);

public Q_SLOTS:
    void OnRequestResizeDesktopVisibleChanged(bool visible);

private:
    QVBoxLayout *m_contentLayout;
    MonitorControlWidget *m_monitorControlWidget;
    ResolutionWidget *m_resolutionWidget;
    RefreshRateWidget *m_refreshRateWidget;
    RotateWidget *m_rotateWidget;

    DisplayModel *m_model;
    Monitor *m_monitor;
};
} // namespace DCC_NAMESPACE

#endif // SECONDARYSCREENDIALOG_H
