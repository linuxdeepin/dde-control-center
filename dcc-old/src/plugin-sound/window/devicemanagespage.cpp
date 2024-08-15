//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "devicemanagespage.h"
#include "soundmodel.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"

#include <DStyle>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DStandardItem>
#include <DListView>

#include <QBoxLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

DevicemanagesPage::DevicemanagesPage(Port::Direction direction, QWidget *parent)
    : QWidget(parent)
    , m_direction(direction)
    , m_layout(new QVBoxLayout(this))
    , m_deviceGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
{
}

DevicemanagesPage::~DevicemanagesPage()
{

}

void DevicemanagesPage::setModel(SoundModel *model)
{
    m_model = model;

    connect(m_model, &SoundModel::portAdded, this, &DevicemanagesPage::addPort);
    connect(m_model, &SoundModel::portRemoved, this, &DevicemanagesPage::removePort);

    initUI();
    refreshPort();
}

void DevicemanagesPage::refreshPort()
{
    auto ports = m_model->ports();
    for (auto port : ports) {
        if (m_direction == port->direction())
            addPort(port);
    }
}

void DevicemanagesPage::addPort(const Port *port)
{
    if (m_devicePort.contains(port)){
        return;
    }

    SwitchWidget *switchDevs = new SwitchWidget(this);
    switchDevs->setTitle(port->name() + "(" + port->cardName() + ")");
    switchDevs->setChecked(port->isEnabled());

    // add Device
    if (m_direction == port->direction()) {
        m_devicePort.append(port);
        m_deviceGroup->appendItem(switchDevs);
    }

    // 切換狀態
    connect(switchDevs, &SwitchWidget::checkedChanged, this, [ = ]{
        if(port != nullptr){
            Q_EMIT requestSwitchSetEnable(port->cardId(), port->id(), switchDevs->checked());
        } else {
            switchDevs->setChecked(false);
            switchDevs->deleteLater();
        }
    });

    // 弹出横幅 控制状态
    connect(port, &Port::currentPortEnabled, switchDevs, &SwitchWidget::setChecked);
}

void DevicemanagesPage::removePort(const QString &portId, const uint &cardId)
{
    // TODO: 修改删除逻辑
    for (int i = 0; i < m_devicePort.size(); i++) {
        if (m_devicePort.at(i)->id() == portId && m_devicePort.at(i)->cardId() == cardId){
            m_deviceGroup->removeItem(m_deviceGroup->getItem(i));
            m_devicePort.removeAt(i);
            return;
        }
    }
}

void DevicemanagesPage::initUI()
{
    // 输入设备
    m_deviceGroup->getLayout()->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_deviceGroup);
    m_layout->addStretch();

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    setLayout(m_layout);
}

