/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "datetimewidget.h"
#include "widgets/settingsgroup.h"
#include "clockitem.h"
#include "modules/datetime/datetimemodel.h"

#include <QStandardItemModel>

using namespace dcc::datetime;

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DatetimeWidget::DatetimeWidget(QWidget *parent)
    : QWidget(parent)
    , m_listview(new QListView)
    , m_model(nullptr)
    , m_clockItem(new ClockItem)
    , m_finalPage(nullptr)
{
    setObjectName("Datetime");

    m_listview->setViewMode(QListView::ListMode);
    m_listview->setResizeMode(QListView::Adjust);
    m_listview->setSpacing(10);

    SettingsGroup *timezoneGroup = new SettingsGroup;

    //default open 24 hour type : set hourTypeSwitch(true) , then set ClockItem TimeHourType
    SwitchWidget *hourTypeSwitch = new SwitchWidget(tr("24 Hour Time"));
    hourTypeSwitch->setChecked(true);
    m_clockItem->setTimeHourType(hourTypeSwitch->checked());

    timezoneGroup->appendItem(m_clockItem);

    auto model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), "Timezone List"));
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), "System Timezone")),
          model->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "时间设置")),
          m_listview->setModel(model);

    connect(m_listview, &QListView::pressed, this, &DatetimeWidget::onItemClieck);

    layout = new QVBoxLayout;
    layout->addWidget(timezoneGroup);
    layout->addWidget(hourTypeSwitch);
    layout->addWidget(m_listview);
    this->setLayout(layout);

    // true : 24 hour type  ,  false : 12 hour type ; All use the system time can recive DatetimeWidget::requestSetHourType signal
    connect(hourTypeSwitch, &SwitchWidget::checkedChanged, this, &DatetimeWidget::requestSetHourType);
    connect(this, &DatetimeWidget::requestSetHourType, this, &DatetimeWidget::onHourTypeChanged);
}

DatetimeWidget::~DatetimeWidget()
{

}

void DatetimeWidget::init()
{

}

void DatetimeWidget::setModel(const DatetimeModel *model)
{
    m_model = model;

    connect(model, &DatetimeModel::NTPChanged, [this](const bool & ntp) {
        Q_EMIT requestSetNtp(ntp);
    });
}

void DatetimeWidget::onItemClieck(const QModelIndex &index)
{
//    qDebug() << " index : " << index.row();
    Q_EMIT requestPushWidget(index.row());
}

void DatetimeWidget::onHourTypeChanged(const bool &ntp)
{
    if (m_clockItem) {
        m_clockItem->setTimeHourType(ntp);
    }
}



