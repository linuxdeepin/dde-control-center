/*
 * Copyright (C) 2011 ~ 2020 Uniontech Technology Co., Ltd.
 *
 * Author:     xiehui <xiehui@uniontech.com>
 *
 * Maintainer: xiehui <xiehui@uniontech.com>
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

#include "airplanemodepage.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/contentwidget.h"
#include "window/utils.h"

#include <networkmodel.h>

#include <QVBoxLayout>
#include <QDBusConnection>

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
