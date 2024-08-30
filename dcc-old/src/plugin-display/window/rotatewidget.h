//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


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
