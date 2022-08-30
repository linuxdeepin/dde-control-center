// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    ~SecondaryScreenDialog();

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
    void requestSetFillMode(dcc::display::Monitor *monitor, const QString fillMode);
    void requestSetRotate(dcc::display::Monitor *monitor, const int rotate);
    void requestGatherWindows(const QPoint cursor);
    void requestGatherEnabled(const bool enable);
    void requestCloseRecognize();
    void requestCurrFillModeChanged(dcc::display::Monitor *monitor, const QString fillMode);

public Q_SLOTS:
    void OnRequestResizeDesktopVisibleChanged(bool visible);

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
