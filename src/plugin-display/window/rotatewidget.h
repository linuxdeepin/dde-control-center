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

#include "interface/namespace.h"
#include "widgets/settingsitem.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QHBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class Monitor;
class DisplayModel;

class RotateWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit RotateWidget(int comboxWidth = 300, QWidget *parent = nullptr);

public:
    void setModel(DisplayModel *model, Monitor *monitor);
    void setMonitor(Monitor *monitor);

Q_SIGNALS:
    void requestSetRotate(Monitor *monitor, const int rotate);

private:
    void initRotate();

private:
    QHBoxLayout *m_contentLayout;
    QLabel *m_rotateLabel;
    QComboBox *m_rotateCombox;

    DisplayModel *m_model;
    Monitor *m_monitor;
};
}

#endif // ROTATEWIDGET_H
