//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef COLLABORATIVELINKWIDGET_H
#define COLLABORATIVELINKWIDGET_H

#include "treecombox.h"
#include "interface/namespace.h"

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
    void requestCurrentMachineConnect(Machine * machine);
    void requestCurrentDeviceSharingConnect(Machine * machine);
    void requestCurrentMachineDisconnect(Machine *mac);
    void requsetCooperation(Machine *machine);

    void requestOpenSharedDevices(bool open);
    void requestOpenSharedClipboard(bool open);
    void requestFilesStoragePath(const QString &path);

    void requestFlowDirection(Machine *machine, const int& idx);

public Q_SLOTS:
    void disconnectMachine();
    void changeTreeComboxIndex(const QModelIndex &index);
    void changeDirectionComboxIndex(const int idx);

private:
    void initMachine();
    void addMachine(Machine * machine);
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

    // 连接方向
    DCC_NAMESPACE::SettingsItem *m_directionComboxItem;
    DTK_WIDGET_NAMESPACE::DComboBox *m_directionCombox;
    QStandardItemModel *m_directionComboxModel;

    Machine *m_currentMachineDevcice;
};

}

#endif // COLLABORATIVELINKWIDGET_H
