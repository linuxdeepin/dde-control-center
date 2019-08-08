/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "multiscreensettingpage.h"

#include "widgets/basiclistview.h"
#include "displaycontrolmodel.h"
#include "displayitemdelegate.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE::display;
using namespace dcc::widgets;
using namespace dcc::display;

MultiScreenSettingPage::MultiScreenSettingPage(QWidget *parent)
    : QWidget(parent)
    , m_baseListView(new BasicListView)
{
    m_baseListView->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(m_baseListView);
    mainLayout->setMargin(0);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    setMinimumWidth(300);

    connect(m_baseListView, &BasicListView::clicked,
            this, &MultiScreenSettingPage::onItemClicked);
}

void MultiScreenSettingPage::setModel(dcc::display::DisplayModel *model)
{
    m_baseListView->setModel(new DisplayControlModel(model));
    m_baseListView->setItemDelegate(new DisplayItemDelegate);
}

void MultiScreenSettingPage::onItemClicked(const QModelIndex &index)
{
    const DisplayControlModel::ItemType type =
        index.data(DisplayControlModel::ItemTypeRole).value<DisplayControlModel::ItemType>();

    switch (type) {
    case DisplayControlModel::Duplicate:
        Q_EMIT requestDuplicateMode();
        return;
    case DisplayControlModel::Extend:
        Q_EMIT requestExtendMode();
        return;
    case DisplayControlModel::Custom:
        Q_EMIT requestConfig(index.data(DisplayControlModel::ItemConfigNameRole).toString());
        return;
    case DisplayControlModel::NewConfig:
        Q_EMIT requestCustom();
        return;
    default:;
    }

    Q_ASSERT(type == DisplayControlModel::Specified);

    Q_EMIT requestOnlyMonitor(index.data(DisplayControlModel::ItemNameRole).toString());
}
