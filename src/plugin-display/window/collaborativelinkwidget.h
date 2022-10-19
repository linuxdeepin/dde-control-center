/*
 * Copyright (C) 2011 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef COLLABORATIVELINKWIDGET_H
#define COLLABORATIVELINKWIDGET_H

#include <QStandardItem>
#include <QWidget>
#include "interface/plugininterface.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE


namespace DCC_NAMESPACE {
class TitleLabel;
class SwitchWidget;
class SettingsGroup;
class SettingsItem;
class DisplayModel;
class Machine;

// 协同链接
class CollaborativeLinkWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CollaborativeLinkWidget(QWidget *parent = nullptr);
    ~CollaborativeLinkWidget();

public:
    void initUI();
    void initConnect();
    void setModel(DisplayModel *model);

Q_SIGNALS:
    void requestCooperationEnable(const bool enabled);
    void requestCurrentMachinePair(Machine * machine);
    void requestCurrentMachineDisconnect(Machine *mac);

public Q_SLOTS:
    void disconnectMachine();
    void changeComboxIndex(const int idx);

private:
    void initMachine();
    void addMachine(Machine * machine);
    void cooperationStatusChanged(bool status);

private:
    DisplayModel *m_displayModel;

    QVBoxLayout *m_centralLayout;
    DCC_NAMESPACE::TitleLabel *m_deviceTitle;
    DCC_NAMESPACE::SettingsGroup *m_settingsGroup;
    DCC_NAMESPACE::SettingsItem *m_deviceSwitchItem;
    DCC_NAMESPACE::SwitchWidget *m_deviceSwitch;

    DCC_NAMESPACE::SettingsItem *m_deviceComboBoxItem;
    QComboBox *m_deviceCombox;
    QStandardItemModel *m_deviceComboxModel;

    QPushButton *m_deviceButton;

    Machine *m_currentMachineDevcice;
};

}

#endif // COLLABORATIVELINKWIDGET_H
