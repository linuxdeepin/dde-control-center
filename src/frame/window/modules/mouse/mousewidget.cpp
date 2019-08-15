/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#include "mousewidget.h"
#include "window/utils.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"

#include <DStyleOption>

#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPainter>
#include <QVBoxLayout>
#include <QListView>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::widgets;

MouseWidget::MouseWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("Mouse");
    m_contentLayout = new QVBoxLayout(this);
    m_mouseListView = new QListView(this);
    m_contentLayout->addWidget(m_mouseListView);
    init();
    setLayout(m_contentLayout);
    //setStyleSheet("MouseWidget{background: red}");
}

void MouseWidget::init()
{
    m_listviewModel = new QStandardItemModel(m_mouseListView);

    QStringList menuList;
    menuList << tr("General") << tr("Mouse") << tr("TouchPad") << tr("TrackPoint");

    QStandardItem *mouseItem = nullptr;
    for (auto strMenuItem : menuList) {
        mouseItem = new QStandardItem(strMenuItem);
        mouseItem->setData(Dtk::RoundedBackground, Dtk::BackgroundTypeRole);
        mouseItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_listviewModel->appendRow(mouseItem);
    }
    m_mouseListView->setFrameShape(QFrame::NoFrame);
    m_mouseListView->setModel(m_listviewModel);
    m_mouseListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mouseListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_mouseListView, &QListView::clicked, this, &MouseWidget::onItemClieck);
}

void MouseWidget::initSetting()
{
    m_mouseListView->setCurrentIndex(m_listviewModel->index(0, 0));
    m_mouseListView->clicked(m_listviewModel->index(0, 0));
}

void MouseWidget::onItemClieck(const QModelIndex &index)
{
    // qDebug() << "row: " << index.row() << " column: " << index.column();
    switch (index.row()) {
    case 0:
        Q_EMIT showGeneralSetting();
        break;
    case 1:
        Q_EMIT showMouseSetting();
        break;
    case 2:
        Q_EMIT showTouchpadSetting();
        break;
    case 3:
        Q_EMIT showTrackPointSetting();
        break;
    default:
        Q_EMIT showGeneralSetting();
        break;
    }

}

//void MouseWidget::paintEvent(QPaintEvent *event)
//{
//    QPainter pa(this);

//    //pa.fillRect(rect(), Qt::red);
//}
