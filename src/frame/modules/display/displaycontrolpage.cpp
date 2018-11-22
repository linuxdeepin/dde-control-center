/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "displaycontrolpage.h"
#include "widgets/basiclistview.h"
#include "displaycontrolmodel.h"
#include "displayitemdelegate.h"

#include <QEvent>
#include <QVBoxLayout>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::display;

DisplayControlPage::DisplayControlPage(QWidget *parent) : SettingsItem(parent)
{
    m_baseListView  = new BasicListView;
    m_baseListView->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(m_baseListView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(m_baseListView, &BasicListView::clicked, this, &DisplayControlPage::onItemClicked);
}

void DisplayControlPage::setModel(DisplayModel *model)
{
    m_baseListView->setModel(new DisplayControlModel(model));
    m_baseListView->setItemDelegate(new DisplayItemDelegate);
}

void DisplayControlPage::onItemClicked(const QModelIndex &index) const
{
    const DisplayControlModel::ItemType type =
        index.data(DisplayControlModel::ItemTypeRole).value<DisplayControlModel::ItemType>();

    switch (type) {
        case DisplayControlModel::Duplicate: Q_EMIT requestDuplicateMode(); return;
        case DisplayControlModel::Extend: Q_EMIT    requestExtendMode(); return;
        case DisplayControlModel::Custom:
            Q_EMIT requestConfig(index.data(DisplayControlModel::ItemConfigNameRole).toString());
            return;
        case DisplayControlModel::NewConfig: Q_EMIT requestCustom(); return;
        default:;
    }

    Q_ASSERT(type == DisplayControlModel::Specified);

    Q_EMIT requestOnlyMonitor(index.data(DisplayControlModel::ItemNameRole).toString());
}
