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
#include "window/gsettingwatcher.h"
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
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    init();

    //~ contents_path /defapp/Music/Add Application
    //~ child_page Music
    tr("Add Application");
}

DatetimeWidget::~DatetimeWidget()
{
    requestCloseWidget();
}

void DatetimeWidget::init()
{
    QList<QPair<QString, QString>> menuIconText;
    menuIconText = {
        { "dcc_time_zone", tr("Timezone List")},
        { "dcc_setting", tr("Time Settings")} ,
        { "dcc_time_format", tr("Time Format")}
    };

    DStandardItem *item = nullptr;
    auto model = new QStandardItemModel(this);

    m_listview->setAccessibleName("List_datetimesettingList");
    m_listview->setFrameShape(QFrame::NoFrame);
    m_listview->setModel(model);
    m_listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listview->setCurrentIndex(model->index(0, 0));
    m_listview->setItemSpacing(10);
    m_listview->setViewportMargins(0, 0, 0, 0);
    m_listview->setIconSize(ListViweIconSize);

    m_itemList.append({menuIconText.at(0).first, menuIconText.at(0).second,QMetaMethod::fromSignal(&DatetimeWidget::requestTimezoneList), nullptr, "timezoneList"});
    m_itemList.append({menuIconText.at(1).first, menuIconText.at(1).second,QMetaMethod::fromSignal(&DatetimeWidget::requestTimeSetting), nullptr, "timeSettings"});
    m_itemList.append({menuIconText.at(2).first, menuIconText.at(2).second,QMetaMethod::fromSignal(&DatetimeWidget::requestFormatSetting), nullptr, "timeFormat"});

    for (auto it = m_itemList.cbegin(); it != m_itemList.cend(); ++it) {
        item = new DStandardItem(QIcon::fromTheme(it->itemIcon), it->itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        model->appendRow(item);
        GSettingWatcher::instance()->bind(it->gsettingsName, m_listview, item);
    }

    if(InsertPlugin::instance()->needPushPlugin("datetime"))
        InsertPlugin::instance()->pushPlugin(model,m_itemList);

    //default open 24 hour type : set hourTypeSwitch(true) , then set ClockItem TimeHourType
    //~ contents_path /datetime
    m_hourTypeSwitch = new SwitchWidget(tr("24-hour Time"));
    m_hourTypeSwitch->setContentsMargins(0, 0, 0, 0);
    m_hourTypeSwitch->addBackground();
    m_clockItem->setTimeHourType(m_hourTypeSwitch->checked());
    m_clockItem->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(10);
    layout->addWidget(m_clockItem);
    layout->addWidget(m_hourTypeSwitch);
    layout->addWidget(m_listview);
    setLayout(layout);

    connect(m_listview, &DListView::clicked, this, &DatetimeWidget::onItemClicked);
    connect(m_listview, &DListView::activated, m_listview, &QListView::clicked);
    // true : 24 hour type  ,  false : 12 hour type ; All use the system time can recive DatetimeWidget::requestSetHourType signal
    connect(m_hourTypeSwitch, &SwitchWidget::checkedChanged, this, &DatetimeWidget::requestSetHourType);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row, const QString & name) {
        //不是本模块配置不响应
        if (!configContent(name))
            return ;
        bool isAllHidden = true;
        for (int i = 0; i < m_listview->model()->rowCount(); i++) {
            if (!m_listview->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_listview->selectionModel()->selectedRows().size() > 0) {
            int index = m_listview->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHidden) {
            m_lastIndex = QModelIndex();
            m_listview->clearSelection();
        }
    });
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

    for(int i = 0; i < m_listview->model()->rowCount(); i++) {
        if (!m_listview->isRowHidden(i)) {
            m_listview->activated(m_listview->model()->index(i, 0));
            break;
        }
    }
}

void DatetimeWidget::onItemClicked(const QModelIndex &index)
{
    if (m_lastIndex == index) return;

    m_lastIndex = index;

    m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].pulgin ? m_itemList[index.row()].pulgin : this);
    //Q_EMIT requestPushWidget(index.row());

    m_listview->resetStatus(index);
}

void DatetimeWidget::onHourTypeChanged(const bool &type)
{
    if (m_clockItem) {
        m_hourTypeSwitch->setChecked(type);
        m_clockItem->setTimeHourType(type);
    }
}

bool DatetimeWidget::configContent(const QString &configName)
{
    for (auto m : m_itemList) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}
