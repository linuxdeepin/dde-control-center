/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <190771752ri@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <190771752ri@gmail.com>
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

#include "navwidget.h"
#include "navdelegate.h"

#include <QHeaderView>
#include <QVBoxLayout>

NavWidget::NavWidget(QWidget *parent)
    : QWidget(parent)
{
    m_navView = new QTableView;
    m_navModel = new NavModel;
    m_navView->setModel(m_navModel);
    m_navView->setItemDelegate(new NavDelegate);

    m_navView->setMouseTracking(true);
    m_navView->setSelectionMode(QTableView::NoSelection);
    m_navView->horizontalHeader()->setVisible(false);
    m_navView->verticalHeader()->setVisible(false);
    m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_navView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_navView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_navView->setContentsMargins(0, 0, 0, 0);
    m_navView->setFrameStyle(QFrame::NoFrame);
    m_navView->setShowGrid(false);

    QVBoxLayout *centralLayout = new QVBoxLayout;

    centralLayout->addWidget(m_navView);
    centralLayout->setSpacing(1);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);

    connect(m_navView, &QTableView::entered, this, &NavWidget::onNavEnter);
    connect(m_navView, &QTableView::clicked, this, &NavWidget::onNavClicked);
    connect(this, &NavWidget::requestModuleVisible, m_navModel, &NavModel::setModuleVisible);
}

void NavWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    m_navModel->setHoverIndex(QModelIndex());
}

void NavWidget::onNavEnter(const QModelIndex &index)
{
    m_navModel->setHoverIndex(index);
}

void NavWidget::onNavClicked(const QModelIndex &index)
{
    const QString &itemName = index.data(Qt::WhatsThisRole).toString();
    if (!itemName.isEmpty()) {
        requestModule(itemName);
    }
}
