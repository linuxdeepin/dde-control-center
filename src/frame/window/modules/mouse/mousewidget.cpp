/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "../../../modules/mouse/mousemodel.h"
#include "../../../modules/mouse/widget/palmdetectsetting.h"
#include "widgets/dccslider.h"
#include "../../../modules/mouse/widget/doutestwidget.h"

#include <QPushButton>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPainter>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::widgets;
MouseWidget::MouseWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("Mouse");
    //setTitle(tr("Mouse and Touchpad"));
    m_contentLayout = new QVBoxLayout(this);
    m_mouseListView = new QListView(this);
    m_contentLayout->addWidget(m_mouseListView);
    init();
    setLayout(m_contentLayout);
    //setStyleSheet("MouseWidget{background: red}");
}

void MouseWidget::init()
{
//    m_mouseListView->setViewMode(QListView::IconMode);
    auto model = new QStandardItemModel(m_mouseListView);

    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), "General"));
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), "Mouse"));
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "TouchPad"));
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "PalmDetect"));
    m_mouseListView->setModel(model);
//    m_mouseListView->setFlow(QListView::TopToBottom);
//    m_mouseListView->setWordWrap(true);
    m_mouseListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_mouseListView, &QListView::clicked, this, &MouseWidget::onItemClieck);
}

void MouseWidget::onItemClieck(const QModelIndex &index)
{
    qDebug() << "row: " << index.row() << " column: " << index.column();
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
