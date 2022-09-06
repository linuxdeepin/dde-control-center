// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "devicemanagespage.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "modules/sound/soundmodel.h"
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

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;
DWIDGET_USE_NAMESPACE

DevicemanagesPage::DevicemanagesPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_outputGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_inputGroup(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
{
    setAccessibleName("DevicemanagesPage");
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
    refreshTitleStatus(Port::Out);
    refreshTitleStatus(Port::In);
}

void DevicemanagesPage::refreshPort()
{
    auto ports = m_model->ports();
    for (auto port : ports) {
        addPort(port);
    }
}

void DevicemanagesPage::addPort(const Port *port)
{
    if (m_inputPort.contains(port) || m_outputPort.contains(port)){
        return;
    }

    SwitchWidget *switchDevs = new SwitchWidget(this);
    switchDevs->setTitle(port->name() + "(" + port->cardName() + ")");
    switchDevs->setChecked(port->isEnabled());

    // add Device
    if (Port::Out == port->direction()) {
        m_outputPort.append(port);
        m_outputGroup->appendItem(switchDevs);
        refreshTitleStatus(port->direction());
    } else {
        m_inputPort.append(port);
        m_inputGroup->appendItem(switchDevs);
        refreshTitleStatus(port->direction());
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
    for (int i = 0; i < m_inputPort.size(); i++) {
        if (m_inputPort.at(i)->id() == portId && m_inputPort.at(i)->cardId() == cardId){
            m_inputGroup->removeItem(m_inputGroup->getItem(i));
            m_inputPort.removeAt(i);
            refreshTitleStatus(Port::In);
            return;
        }
    }

    for (int i = 0; i < m_outputPort.size(); i++) {
        if (m_outputPort.at(i)->id() == portId && m_outputPort.at(i)->cardId() == cardId) {
            m_outputGroup->removeItem(m_outputGroup->getItem(i));
            m_outputPort.removeAt(i);
            refreshTitleStatus(Port::Out);
            return;
        }
    }
}

void DevicemanagesPage::initUI()
{
    // TODO: 输出设备
    const int titleLeftMargin = 8;
    //~ contents_path /sound/Devices
    //~ child_page Devices
    m_outputDeviceTitle = new TitleLabel(tr("Output Devices"));
    DFontSizeManager::instance()->bind(m_outputDeviceTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_outputDeviceTitle->setContentsMargins(titleLeftMargin, 0, 0, 0);
    m_outputDeviceTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_outputDeviceTitle->adjustSize();

    m_outputlblTip = new  DTipLabel(tr("Enable or disable a device as you want"));
    DFontSizeManager::instance()->bind(m_outputlblTip, DFontSizeManager::T8);
    m_outputlblTip->adjustSize();
    m_outputlblTip->setWordWrap(true);
    m_outputlblTip->setContentsMargins(titleLeftMargin, 0, 0, 0);
    m_outputlblTip->setAlignment(Qt::AlignLeft);

    m_outputGroup->getLayout()->setContentsMargins(0, 0, 0, 0);
    m_outputGroup->setContentsMargins(0, 0, 0, 0);

    m_layout->addWidget(m_outputDeviceTitle);
    m_layout->addWidget(m_outputlblTip);
    m_layout->addSpacing(8);
    m_layout->addWidget(m_outputGroup);
    m_layout->addSpacing(15);

    // 输入设备
    //~ contents_path /sound/Devices
    //~ child_page Devices
    m_inputDeviceTitle = new TitleLabel(tr("Input Devices"));
    DFontSizeManager::instance()->bind(m_inputDeviceTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_inputDeviceTitle->setContentsMargins(titleLeftMargin, 0, 0, 0);
    m_inputDeviceTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_inputlblTip = new  DTipLabel(tr("Enable or disable a device as you want"));
    DFontSizeManager::instance()->bind(m_inputlblTip, DFontSizeManager::T8);
    m_inputlblTip->adjustSize();
    m_inputlblTip->setWordWrap(true);
    m_inputlblTip->setContentsMargins(titleLeftMargin, 0, 0, 0);
    m_inputlblTip->setAlignment(Qt::AlignLeft);

    m_inputGroup->getLayout()->setContentsMargins(0, 0, 0, 0);
    m_inputGroup->setContentsMargins(0, 0, 0, 0);

    m_layout->addWidget(m_inputDeviceTitle);
    m_layout->addWidget(m_inputlblTip);
    m_layout->addSpacing(8);
    m_layout->addWidget(m_inputGroup);
    m_layout->addStretch();

    m_layout->setContentsMargins(ThirdPageContentsMargins);
    m_layout->setSpacing(0);
    setLayout(m_layout);
}

void DevicemanagesPage::refreshTitleStatus(Port::Direction direction)
{
    if (direction == Port::Out) {
        if (m_outputGroup->itemCount()) {
            m_outputDeviceTitle->show();
            m_outputlblTip->show();
        } else {
            m_outputDeviceTitle->hide();
            m_outputlblTip->hide();
        }
    }

    if (direction == Port::In) {
        if (m_inputGroup->itemCount()) {
            m_inputDeviceTitle->show();
            m_inputlblTip->show();
        }else {
            m_inputDeviceTitle->hide();
            m_inputlblTip->hide();
        }
    }
}

