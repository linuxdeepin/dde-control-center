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
#include "widgets/switchwidget.h"
#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"
#include "clockitem.h"
#include "modules/datetime/datetimemodel.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"
#include "window/utils.h"
#include "widgets/multiselectlistview.h"
#include <QStandardItemModel>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::datetime;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DatetimeWidget::DatetimeWidget(QWidget *parent)
    : QWidget(parent)
    , m_listview(new dcc::widgets::MultiSelectListView(this))
    , m_clockItem(new ClockItem(this))
    , m_model(nullptr)
{
    m_listview->setAccessibleName("settinglist");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    init();
}

DatetimeWidget::~DatetimeWidget()
{
    requestCloseWidget();
}

void DatetimeWidget::init()
{
    QList<QPair<QIcon, QString>> menuIconText;
    menuIconText = {
        { QIcon::fromTheme("dcc_time_zone"), tr("Timezone List")},
        //~ contents_path /datetime/Time Settings
        { QIcon::fromTheme("dcc_setting"), tr("Time Settings")}
    };

    DStandardItem *item = nullptr;
    auto model = new QStandardItemModel(this);
    for (auto it = menuIconText.cbegin(); it != menuIconText.cend(); ++it) {
        item = new DStandardItem(it->first, it->second);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        model->appendRow(item);
    }
    m_listview->setAccessibleName("List_datetimesettingList");
    m_listview->setFrameShape(QFrame::NoFrame);
    m_listview->setModel(model);
    m_listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listview->setCurrentIndex(model->index(0, 0));
    m_listview->setItemSpacing(10);
    m_listview->setViewportMargins(ScrollAreaMargins);

    //default open 24 hour type : set hourTypeSwitch(true) , then set ClockItem TimeHourType
    //~ contents_path /datetime/Timezone List
    m_hourTypeSwitch = new SwitchWidget(tr("24-hour Time"));
    m_hourTypeSwitch->addBackground();
    m_clockItem->setTimeHourType(m_hourTypeSwitch->checked());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->addWidget(m_clockItem);
    layout->addWidget(m_hourTypeSwitch);
    layout->addWidget(m_listview);
    setLayout(layout);

    connect(m_listview, &DListView::clicked, this, &DatetimeWidget::onItemClicked);
    connect(m_listview, &DListView::activated, m_listview, &QListView::clicked);
    // true : 24 hour type  ,  false : 12 hour type ; All use the system time can recive DatetimeWidget::requestSetHourType signal
    connect(m_hourTypeSwitch, &SwitchWidget::checkedChanged, this, &DatetimeWidget::requestSetHourType);
}

void DatetimeWidget::setModel(const DatetimeModel *model)
{
    m_model = model;
}

DListView *DatetimeWidget::getListViewPointer()
{
    return m_listview;
}

void DatetimeWidget::setCurrentTimeZone(const ZoneInfo &info)
{
    m_clockItem->setTimeZone(info);
}

void DatetimeWidget::setTimeType(const QString &country)
{
    bool flg = false;

    if (country.contains("en_", Qt::CaseInsensitive)) {
        flg = true;
    }
    m_clockItem->setTimeEnglishType(flg);
}

void DatetimeWidget::setDefaultWidget()
{
    m_listview->setCurrentIndex(m_listview->model()->index(0, 0));
    m_listview->clicked(m_listview->model()->index(0, 0));
}

void DatetimeWidget::onItemClicked(const QModelIndex &index)
{
    Q_EMIT requestPushWidget(index.row());
    m_listview->resetStatus(index);
}

void DatetimeWidget::onHourTypeChanged(const bool &type)
{
    if (m_clockItem) {
        m_hourTypeSwitch->setChecked(type);
        m_clockItem->setTimeHourType(type);
    }
}
