//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "collaborativelinkwidget.h"
#include "cooperationsettingsdialog.h"
#include "treecombox.h"

#include "operation/displaymodel.h"
#include "operation/machine.h"

#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"

#include <DComboBox>
#include <DStandardItem>

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTreeView>
#include <QWidgetAction>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

CollaborativeLinkWidget::CollaborativeLinkWidget(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
    , m_deviceTitle(new TitleLabel(tr("Multi-Screen Collaboration"), this))
    , m_settingsGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_deviceSwitchItem(new SettingsItem(this))
    , m_deviceSwitch(new SwitchWidget(this))
    , m_deviceComboBoxItem(new SettingsItem(this))
    , m_deviceComboxModel(new QStandardItemModel(this))
    , m_deviceCombox(new TreeCombox(m_deviceComboxModel, m_deviceComboBoxItem))
    , m_deviceButton(new QPushButton(this))
    , m_directionComboxItem(new SettingsItem(this))
    , m_directionCombox(new DTK_WIDGET_NAMESPACE::DComboBox(m_directionComboxItem))
    , m_directionComboxModel(new QStandardItemModel(m_directionCombox))
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
    deviceLabel->setText(tr("PC Collaboration"));
    switchLayout->addWidget(deviceLabel);
    switchLayout->addWidget(m_deviceSwitch);
    m_deviceSwitchItem->setLayout(switchLayout);

    QHBoxLayout *comboxLayout = new QHBoxLayout(m_deviceComboBoxItem);
    QLabel *devLabel = new QLabel(this);
    devLabel->setText(tr("Connect to"));
    m_deviceButton->setFixedWidth(36);
    m_deviceButton->setIcon(QIcon::fromTheme("dcc_break"));
    m_deviceButton->setEnabled(false);

    m_deviceCombox->setModel(m_deviceComboxModel);
    m_deviceCombox->setEditText(tr("Select a device for collaboration"));
    comboxLayout->addWidget(devLabel);
    comboxLayout->addStretch(1);
    comboxLayout->addWidget(m_deviceCombox, 1);
    comboxLayout->addWidget(m_deviceButton);

    QHBoxLayout *directionLayout = new QHBoxLayout(m_directionComboxItem);
    QLabel *directionLabel = new QLabel(this);
    directionLabel->setText(tr("Device Orientation"));
    m_directionCombox->setModel(m_directionComboxModel);
    directionLayout->addWidget(directionLabel);
    directionLayout->addWidget(m_directionCombox);
    initDirectionItem();
    m_directionComboxItem->setLayout(directionLayout);

    m_settingsGroup->appendItem(m_deviceSwitchItem);
    m_settingsGroup->appendItem(m_deviceComboBoxItem);
    m_settingsGroup->appendItem(m_directionComboxItem);
    m_centralLayout->addWidget(m_settingsGroup);

    setLayout(m_centralLayout);
}

void CollaborativeLinkWidget::initConnect()
{
    // TODO: 协同开关
    connect(m_deviceSwitch, &SwitchWidget::checkedChanged, this, &CollaborativeLinkWidget::requestCooperationEnable);
    connect(m_deviceSwitch, &SwitchWidget::checkedChanged, m_deviceCombox, &QWidget::setEnabled);
    connect(m_deviceButton, &QPushButton::clicked, this, &CollaborativeLinkWidget::disconnectMachine);
    connect(m_deviceCombox, &TreeCombox::viewItemPressed, this, &CollaborativeLinkWidget::changeTreeComboxIndex);

    // 对话框

    connect(m_directionCombox, static_cast<void (DComboBox::*)(int)>(&DComboBox::activated), this, &CollaborativeLinkWidget::changeDirectionComboxIndex, Qt::QueuedConnection);
}

void CollaborativeLinkWidget::setModel(DisplayModel *model)
{
    m_displayModel = model;

    connect(model, &DisplayModel::deviceSharingSwitchChanged, m_deviceSwitch, &SwitchWidget::setChecked);
    m_deviceSwitch->setChecked(m_displayModel->DeviceSharingSwitch());
    m_deviceCombox->setEnabled(m_deviceSwitch->checked());
    m_directionComboxItem->setVisible(m_displayModel->DeviceSharingSwitch());

    refreshRowItem();
    connect(m_displayModel, &DisplayModel::machinesListChanged, this, [=](){
        if (m_currentMachineDevcice) {
            refreshRowItem();
        } else {
            refreshRowItem();
        }
    });

    if (m_currentMachineDevcice)
        cooperationStatusChanged(m_currentMachineDevcice->deviceSharing());

    qDebug() << " CooperationSettingsDialog settings: " << m_displayModel->SharedDevices() << m_displayModel->SharedClipboard();
}

void CollaborativeLinkWidget::disconnectMachine()
{
    cooperationStatusChanged(false);
    Q_EMIT requestCurrentMachineDisconnect(m_currentMachineDevcice);
}

void CollaborativeLinkWidget::changeTreeComboxIndex(const QModelIndex &index)
{
    qDebug() << "size: " << m_deviceComboxModel->rowCount() << index.row();
    if (m_deviceComboxModel->rowCount() <= 1) return;

    // 处理更多配置项
    if (index.row() == m_deviceComboxModel->rowCount() - 1) {
        CooperationSettingsDialog dialog;
        connect(m_displayModel, &DisplayModel::sharedDevicesChanged, &dialog, &CooperationSettingsDialog::setOpenSharedDevices);
        connect(m_displayModel, &DisplayModel::sharedClipboardChanged, &dialog, &CooperationSettingsDialog::setOpenSharedClipboard);
        connect(m_displayModel, &DisplayModel::filesStoragePathChanged, &dialog, &CooperationSettingsDialog::setFilesStoragePath);
        dialog.setOpenSharedDevices(m_displayModel->SharedDevices());
        dialog.setOpenSharedClipboard(m_displayModel->SharedClipboard());
        dialog.setFilesStoragePath(m_displayModel->filesStoragePath());
        dialog.setButtonDisabled();
        if (dialog.exec() == QDialog::Accepted) {
            if (m_displayModel->SharedClipboard() != dialog.shearClipboardSwitch()->checked()){
                emit requestOpenSharedClipboard(dialog.shearClipboardSwitch()->checked());}
            if (m_displayModel->SharedDevices() != dialog.mousekeyboardSwitch()->checked()){
                emit requestOpenSharedDevices(dialog.mousekeyboardSwitch()->checked());}
            if (m_displayModel->filesStoragePath() != dialog.storagePath())
                emit requestFilesStoragePath(dialog.storagePath());
        }
        return;
    }

    auto tmp = m_deviceComboxModel->index(index.row(), 0);
    auto machine = m_deviceComboxModel->data(tmp, Qt::WhatsThisPropertyRole).value<Machine*>();
    m_currentMachineDevcice = machine;
    if (m_currentMachineDevcice) {
        cooperationStatusChanged(m_currentMachineDevcice->deviceSharing());
    } else {
        cooperationStatusChanged(false);
    }

    if (machine)
        Q_EMIT requestCurrentMachineConnect(machine);
}

void CollaborativeLinkWidget::changeDirectionComboxIndex(const int idx)
{
    qDebug() << "变换方向" << idx;
    if (m_currentMachineDevcice)
        Q_EMIT requestFlowDirection(m_currentMachineDevcice, idx);
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
    m_deviceCombox->addDeviceCheckedIcon(pi, machine->deviceSharing());
    pi->setText(machine->Name() + "(" + machine->IP() + ")");
    pi->setData(QVariant::fromValue<Machine *>(machine), Qt::WhatsThisPropertyRole);

    connect(machine, &Machine::nameChanged, this, [=](const QString& name){
        pi->setText(name + "(" + machine->IP() + ")");
    });

    connect(machine, &Machine::IPChanged, this, [=](const QString& IP){
        pi->setText(machine->Name() + "(" + IP + ")");
    });

    connect(machine, &Machine::deviceSharingChanged, this, [=](const bool deviceSharing){
        deviceSharing ? m_currentMachineDevcice = machine : m_currentMachineDevcice = nullptr;
        cooperationStatusChanged(deviceSharing);
    });

    connect(machine, &Machine::disconnectStatusChanged, m_deviceCombox, [this](bool status) {
        Q_UNUSED(status)
        m_currentMachineDevcice = nullptr;
        cooperationStatusChanged(false);
    });

    connect(machine, &Machine::connectedChanged, m_deviceCombox, [this, machine](bool status) {
        if (status) {
            Q_EMIT requestCurrentDeviceSharingConnect(machine);
        } else {
            cooperationStatusChanged(false);
        }
    });

    connect(machine, &Machine::directionChanged, m_directionCombox, [this](int dir){
        m_directionCombox->setCurrentIndex(dir);
    });

    if (machine->deviceSharing()) {
        m_currentMachineDevcice = machine;
        cooperationStatusChanged(true);
    }
    cooperationStatusChanged(machine->deviceSharing());
    m_deviceComboxModel->appendRow(pi);
}

void CollaborativeLinkWidget::cooperationStatusChanged(bool status)
{
    if (m_currentMachineDevcice && status){
        const QString& name = m_currentMachineDevcice->Name() + "(" + m_currentMachineDevcice->IP() + ")";
        qDebug() << "cooperationStatusChanged: " << name;
        m_deviceCombox->setCurrentText(name);
        m_directionCombox->setCurrentIndex(m_currentMachineDevcice->direction());
        m_deviceButton->setEnabled(true);
        m_deviceCombox->updateItemCheckStatus(name, status);
        m_directionComboxItem->setVisible(true);
    } else {
        m_deviceCombox->setCurrentIndex(-1);
        m_deviceCombox->updateItemCheckStatus(QString(), false);
        m_deviceButton->setEnabled(false);
        m_directionComboxItem->setVisible(false);
    }
}

void CollaborativeLinkWidget::directionItemVisible(bool status)
{
    m_directionComboxItem->setVisible(status);
}

/*
enum FlowDirection {
    FLOW_DIRECTION_TOP = 0;
    FLOW_DIRECTION_RIGHT = 1;
    FLOW_DIRECTION_BOTTOM = 2;
    FLOW_DIRECTION_LEFT = 3;
}
*/
void CollaborativeLinkWidget::initDirectionItem()
{
    QStringList dirList {"top", "right", "bottom", "left"};
    QStringList textList { tr("On the top"), tr("On the right"), tr("On the bottom"), tr("On the left")};

    for (int idx = 0; idx < dirList.size(); idx++) {
        m_directionCombox->addItem(QIcon::fromTheme(QString("dcc_display_%1").arg(dirList[idx])), textList[idx]);
    }
}

void CollaborativeLinkWidget::refreshRowItem()
{
    m_deviceCombox->clear();
    m_deviceComboxModel->clear();

    // 是否有设备
    if (m_displayModel->machineList().isEmpty()) {
        return;
    }

    auto machines = m_displayModel->machineList();
    qDebug() << " 获取设备数据： " << machines.size();
    // 判断是否有历史设备
    QList<Machine *> historyDev;
    QList<Machine *> moreDev;
    for (auto machine : machines) {
        qDebug() << " machine->isHistory() " << machine->isHistory();
        machine->isHistory() ? historyDev << machine : moreDev << machine;
    }

    if (!historyDev.isEmpty())
        m_deviceCombox->addDivicesTitel(tr("My Devices"));

    for (auto machine : historyDev) {
        addMachine(machine);
    }

    // 是否添加更多设备
    if (!moreDev.isEmpty())
        m_deviceCombox->addDivicesTitel(tr("Other Devices"));

    for (auto machine : moreDev) {
        addMachine(machine);
    }
    // 添加更多设置
    if (!historyDev.isEmpty() || !moreDev.isEmpty()) {
        m_deviceCombox->addDevicesSettingsItem();
    }
}

