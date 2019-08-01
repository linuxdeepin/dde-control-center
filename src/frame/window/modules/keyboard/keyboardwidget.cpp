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

#include "keyboardwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>
#include <QPainter>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc::widgets;
KeyboardWidget::KeyboardWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("Mouse");
    m_contentLayout = new QVBoxLayout(this);
    m_keyboardListView = new QListView(this);
    m_contentLayout->addWidget(m_keyboardListView);
    init();
    setLayout(m_contentLayout);
    //setStyleSheet("MouseWidget{background: red}");
}

void KeyboardWidget::init()
{
//    m_mouseListView->setViewMode(QListView::IconMode);
    m_listviewModel = new QStandardItemModel(m_keyboardListView);
    m_listviewModel->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), "General"));
    m_listviewModel->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), "Keyboard Layout"));
    m_listviewModel->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "System Language"));
    m_listviewModel->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "ShortCut"));
    m_keyboardListView->setModel(m_listviewModel);
    m_keyboardListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_keyboardListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_keyboardListView, &QListView::clicked, this, &KeyboardWidget::onItemClick);
    QTimer::singleShot(0, this, &KeyboardWidget::initSetting);
}

void KeyboardWidget::initSetting()
{
    m_keyboardListView->setCurrentIndex(m_listviewModel->index(0,0));
    m_keyboardListView->clicked(m_listviewModel->index(0,0));
}

void KeyboardWidget::onItemClick(const QModelIndex &index)
{
    qDebug() << "row: " << index.row() << " column: " << index.column();
    switch (index.row()) {
    case 0:
        Q_EMIT showGeneralSetting();
        break;
    case 1:
        Q_EMIT showKBLayoutSetting();
        break;
    case 2:
        Q_EMIT showSystemLanguageSetting();
        break;
    case 3:
        Q_EMIT showShortCutSetting();
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
