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
#include "widgets/nextpagewidget.h"
#include "modules/datetime/clockitem.h"
#include "clockitem.h"

#include <QStandardItemModel>
using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::datetime;

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DatetimeWidget::DatetimeWidget(QWidget *parent)
    : QWidget(parent)
    , m_ntpSwitch(new SwitchWidget(tr("24小时制")))
    , m_listview(new QListView)
{
    setObjectName("Datetime");

    m_listview->setViewMode(QListView::ListMode);
    m_listview->setResizeMode(QListView::Adjust);
    m_listview->setSpacing(10);

    SettingsGroup *clockGroup = new SettingsGroup;
    ClockItem *m_clockItem = new ClockItem;
    NextPageWidget *m_timezoneItem =new NextPageWidget;
    m_timezoneItem->setTitle(tr("Change System Timezone"));
    clockGroup->appendItem(m_clockItem);
#ifndef DCC_DISABLE_TIMEZONE
//    clockGroup->appendItem(m_timezoneItem);
#endif

    auto model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), "时区列表"));
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), "系统时区")),
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "时间设置")),
    m_listview->setModel(model);

    connect(m_listview, &QListView::pressed, this, &DatetimeWidget::onItemClieck);

    layout = new QVBoxLayout;
    layout->addWidget(clockGroup);
    layout->addWidget(m_ntpSwitch);
    layout->addWidget(m_listview);
    this->setLayout(layout);

    connect(m_ntpSwitch, &SwitchWidget::checkedChanged, this, &DatetimeWidget::requestSetNtp);
}

DatetimeWidget::~DatetimeWidget()
{

}

void DatetimeWidget::init()
{

}

void DatetimeWidget::onItemClieck(const QModelIndex &index)
{
    qDebug() << " index : " << index.row();
}
