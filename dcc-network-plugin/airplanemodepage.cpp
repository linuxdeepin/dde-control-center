// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "airplanemodepage.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/contentwidget.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QDBusConnection>
#include <DTipLabel>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

AirplaneModepage::AirplaneModepage(QWidget *parent)
    : QWidget(parent)
    , m_airplaneModeSwitch(new SwitchWidget)
    , m_airplaninter(new AirplanInter("com.deepin.daemon.AirplaneMode","/com/deepin/daemon/AirplaneMode",QDBusConnection::systemBus(),this))
    , m_lvprofiles(new DListView)
{
    m_airplaneModeSwitch->setTitle(tr("Airplane Mode"));

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_airplaneModeSwitch);

    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollLayout->addWidget(switchGrp);
    DTipLabel *tip = new DTipLabel(tr("Enabling the airplane mode turns off wireless network, personal hotspot and Bluetooth"), switchGrp);
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignLeft);
    tip->setMargin(8);          // 设置tips与列表中文字左对齐
    scrollLayout->addWidget(tip);
    //这里用个QListView占位置
    scrollLayout->addWidget(m_lvprofiles);
    scrollLayout->setSpacing(10);
    scrollLayout->setContentsMargins(ThirdPageContentsMargins);

    QWidget *widget = new QWidget(this);
    widget->setLayout(scrollLayout);
    ContentWidget *contentWidget = new ContentWidget(this);
    contentWidget->layout()->setMargin(0);
    contentWidget->setContent(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(contentWidget);
    setLayout(mainLayout);

    //初始化开关状态
    m_airplaneModeSwitch->setChecked(m_airplaninter->enabled());
    connect(m_airplaneModeSwitch, &SwitchWidget::checkedChanged, this, &AirplaneModepage::AirplaneEnable);
    //接受改变信号,防止在dock修改不同步
    connect(m_airplaninter, &AirplanInter::EnabledChanged, m_airplaneModeSwitch, &dcc::widgets::SwitchWidget::setChecked);
}

void AirplaneModepage::AirplaneEnable(const bool enable) const
{
    m_airplaninter->Enable(enable);
}
