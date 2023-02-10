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

#include "cooperationsettingsdialog.h"
#include "interface/plugininterface.h"
#include "treecombox.h"

#include <DComboBox>

#include <QStandardItem>
#include <QWidget>

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
    void requestCurrentMachineConnect(Machine *machine);
    void requestCurrentDeviceSharingConnect(Machine *machine);
    void requestCurrentMachineDisconnect(Machine *mac);
    void requsetCooperation(Machine *machine);

    void requestOpenSharedDevices(bool open);
    void requestOpenSharedClipboard(bool open);
    void requestFilesStoragePath(const QString &path);

    void requestFlowDirection(Machine *machine, const int &idx);

public Q_SLOTS:
    void disconnectMachine();
    void changeTreeComboxIndex(const QModelIndex &index);
    void changeDirectionComboxIndex(const int idx);

private:
    void initMachine();
    void addMachine(Machine *machine);
    void cooperationStatusChanged(bool status);
    void directionItemVisible(bool status);
    void initDirectionItem();
    void refreshRowItem();

private:
    DisplayModel *m_displayModel;

    QVBoxLayout *m_centralLayout;
    DCC_NAMESPACE::TitleLabel *m_deviceTitle;
    DCC_NAMESPACE::SettingsGroup *m_settingsGroup;
    DCC_NAMESPACE::SettingsItem *m_deviceSwitchItem;
    DCC_NAMESPACE::SwitchWidget *m_deviceSwitch;

    DCC_NAMESPACE::SettingsItem *m_deviceComboBoxItem;

    QStandardItemModel *m_deviceComboxModel;
    TreeCombox *m_deviceCombox;
    QPushButton *m_deviceButton;
    CooperationSettingsDialog *m_moreSettingsDialog;

    // 连接方向
    DCC_NAMESPACE::SettingsItem *m_directionComboxItem;
    DTK_WIDGET_NAMESPACE::DComboBox *m_directionCombox;
    QStandardItemModel *m_directionComboxModel;

    Machine *m_currentMachineDevcice;
};

} // namespace DCC_NAMESPACE

#endif // COLLABORATIVELINKWIDGET_H
