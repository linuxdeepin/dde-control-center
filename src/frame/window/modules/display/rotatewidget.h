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

#ifndef ROTATEWIDGET_H
#define ROTATEWIDGET_H

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

#include "interface/namespace.h"
#include "modules/display/displaymodel.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace display {
class Monitor;
class DisplayModel;
} // namespace display

} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

class RotateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RotateWidget(QWidget *parent = 0);

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
