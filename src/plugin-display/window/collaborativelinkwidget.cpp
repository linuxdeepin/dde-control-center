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

#include "collaborativelinkwidget.h"

#include <widgets/settingsgroup.h>
#include <widgets/settingsitem.h>
#include <widgets/switchwidget.h>
#include <widgets/titlelabel.h>

#include <DStandardItem>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

#include <src/plugin-display/operation/displaymodel.h>
#include <src/plugin-display/operation/machine.h>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

Q_DECLARE_METATYPE(const Machine *)

CollaborativeLinkWidget::CollaborativeLinkWidget(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
    , m_deviceTitle(new TitleLabel(tr("Multi-Screen Collaboration"), this))
    , m_settingsGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_deviceSwitchItem(new SettingsItem)
    , m_deviceSwitch(new SwitchWidget(this))
    , m_deviceComboBoxItem(new SettingsItem)
    , m_deviceCombox(new QComboBox(m_deviceComboBoxItem))
    , m_deviceComboxModel(new QStandardItemModel(m_deviceCombox))
    , m_deviceButton(new QPushButton(this))
    , m_currentMachineDevcice(nullptr)
{
    initUI();
    initConnect();
}

CollaborativeLinkWidget::~CollaborativeLinkWidget()
{

}

void CollaborativeLinkWidget::initUI()
{
    m_centralLayout->setMargin(0);
    m_centralLayout->addWidget(m_deviceTitle);

    QHBoxLayout *switchLayout = new QHBoxLayout(m_deviceSwitchItem);
    QLabel *deviceLabel = new QLabel(this);
    deviceLabel->setText(tr("Connect to"));
    switchLayout->addWidget(deviceLabel);
    switchLayout->addWidget(m_deviceSwitch);
    m_deviceSwitchItem->setLayout(switchLayout);
    m_settingsGroup->appendItem(m_deviceSwitchItem);

    QHBoxLayout *comboxLayout = new QHBoxLayout(m_deviceComboBoxItem);
    m_deviceButton->setFixedWidth(36);
    m_deviceButton->setIcon(QIcon::fromTheme("dcc_break"));
    m_deviceButton->setEnabled(false);

    m_deviceCombox->setModel(m_deviceComboxModel);
    m_deviceCombox->setCurrentText(tr("Select a device for collaboration"));
    comboxLayout->addStretch(1);
    comboxLayout->addWidget(m_deviceCombox, 1);
    comboxLayout->addWidget(m_deviceButton);
    initMachine();
    m_deviceComboBoxItem->setLayout(comboxLayout);
    m_settingsGroup->appendItem(m_deviceComboBoxItem);

    m_centralLayout->addWidget(m_settingsGroup);
    setLayout(m_centralLayout);
}

void CollaborativeLinkWidget::initConnect()
{
    // TODO: 协同开关
    connect(m_deviceSwitch, &SwitchWidget::checkedChanged, this, &CollaborativeLinkWidget::requestCooperationEnable);
    connect(m_deviceCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &CollaborativeLinkWidget::changeComboxIndex, Qt::QueuedConnection);
    connect(m_deviceButton, &QPushButton::clicked, this, &CollaborativeLinkWidget::disconnectMachine);
}

void CollaborativeLinkWidget::setModel(DisplayModel *model)
{
    m_displayModel = model;

    connect(model, &DisplayModel::deviceSharingSwitchChanged, m_deviceSwitch, &SwitchWidget::setChecked);
    m_deviceSwitch->setChecked(m_displayModel->DeviceSharingSwitch());
    auto AddMacFunc = [ = ](){
        auto machines = m_displayModel->machineList();
        qDebug() << " 获取设备数据： " << machines.size();
        for (auto machine : machines) {
            addMachine(machine);
        }
    };

    AddMacFunc();
    connect(m_displayModel, &DisplayModel::machinesListChanged, this, [=](){
        m_deviceComboxModel->clear();
        if (m_currentMachineDevcice) {
            AddMacFunc();
        } else {
            initMachine();
            AddMacFunc();
        }
    });

    if (m_currentMachineDevcice)
        cooperationStatusChanged(m_currentMachineDevcice->Cooperating());
}

void CollaborativeLinkWidget::disconnectMachine()
{
    cooperationStatusChanged(false);
    Q_EMIT requestCurrentMachineDisconnect(m_currentMachineDevcice);
}

void CollaborativeLinkWidget::changeComboxIndex(const int idx)
{
    qDebug() << " ---- changeComboxIndex" << idx << "size: " << m_deviceComboxModel->rowCount();
    if (idx < 0 || m_deviceComboxModel->rowCount() <= 1) return;

    if (idx == 0) {
        disconnectMachine();
        return;
    }

    auto tmp = m_deviceComboxModel->index(idx, 0);
    auto machine = m_deviceComboxModel->data(tmp, Qt::WhatsThisPropertyRole).value<Machine*>();
    m_currentMachineDevcice = machine;
    if (m_currentMachineDevcice) {
        cooperationStatusChanged(m_currentMachineDevcice->Cooperating());
    } else {
        cooperationStatusChanged(false);
    }

    if (machine)
        Q_EMIT requestCurrentMachinePair(machine);
}

void CollaborativeLinkWidget::initMachine()
{
    DStandardItem *pi = new DStandardItem;
    pi->setText(tr("Select a device for collaboration"));
    m_deviceComboxModel->appendRow(pi);
}

void CollaborativeLinkWidget::addMachine(Machine *machine)
{
    DStandardItem *pi = new DStandardItem;
    pi->setText(machine->Name() + "(" + machine->IP() + ")");
    pi->setData(QVariant::fromValue<Machine *>(machine), Qt::WhatsThisPropertyRole);

    connect(machine, &Machine::nameChanged, this, [=](const QString& name){
        pi->setText(name + "(" + machine->IP() + ")");
    });

    connect(machine, &Machine::IPChanged, this, [=](const QString& IP){
        pi->setText(machine->Name() + "(" + IP + ")");
    });

    connect(machine, &Machine::cooperatingChanged, this, [=](const bool cooperating){
        if (!cooperating)
            m_currentMachineDevcice = nullptr;
        cooperationStatusChanged(cooperating);

    });

    connect(machine, &Machine::disconnnectStatusChanged, m_deviceCombox, [this](bool status) {
        m_currentMachineDevcice = nullptr;
        cooperationStatusChanged(false);
    });

    if (machine->Cooperating()) {
        m_currentMachineDevcice = machine;
        cooperationStatusChanged(true);
    }
    cooperationStatusChanged(machine->Cooperating());
    m_deviceComboxModel->appendRow(pi);
}

void CollaborativeLinkWidget::cooperationStatusChanged(bool status)
{
    if (m_currentMachineDevcice && status){
        m_deviceCombox->setCurrentText(m_currentMachineDevcice->Name() + "(" + m_currentMachineDevcice->IP() + ")");
        m_deviceButton->setEnabled(true);
    } else {
        m_deviceCombox->setCurrentText((tr("请选择协同设备")));
        m_deviceButton->setEnabled(false);
    }
}
