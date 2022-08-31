// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef ROTATEWIDGET_H
#define ROTATEWIDGET_H

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QHBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

using namespace dcc::widgets;

namespace DCC_NAMESPACE {

namespace display {

class RotateWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit RotateWidget(int comboxWidth = 300, QWidget *parent = nullptr);

public:
    void setModel(dcc::display::DisplayModel *model, dcc::display::Monitor *monitor);
    void setMonitor(dcc::display::Monitor *monitor);

Q_SIGNALS:
    void requestSetRotate(dcc::display::Monitor *monitor, const int rotate);

private:
    void initRotate();

private:
    QHBoxLayout *m_contentLayout;
    QLabel *m_rotateLabel;
    QComboBox *m_rotateCombox;

    dcc::display::DisplayModel *m_model;
    dcc::display::Monitor *m_monitor;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // ROTATEWIDGET_H
