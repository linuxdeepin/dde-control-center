/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#include "mainwindow.h"
#include "constant.h"
#include "navwinview.h"
#include "navigation/navmodel.h"

#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    // 初始化视图和布局结构
    QWidget *content = new QWidget(this);
    m_rightView = new QWidget(this);

    m_contentLayout = new QHBoxLayout(content);
    m_rightContentLayout = new QHBoxLayout(m_rightView);
    m_navView = new NavWinView(this);

    m_contentLayout->addWidget(m_navView);
    m_contentLayout->addWidget(m_rightView);

    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_rightContentLayout->setContentsMargins(10, 10, 10, 10);
    m_rightContentLayout->setSpacing(10);

    m_rightView->hide();
    m_rightView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setCentralWidget(content);

    // 初始化测试数据
    m_navModel = new NavModel(1, this);

    // 初始化导航栏名，Theme暂时使用临时的
//    m_navModel-> ->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), m_navModel->transModuleName(MODULES.at(0))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), m_navModel->transModuleName(MODULES.at(1))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), m_navModel->transModuleName(MODULES.at(2))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("deepin-feedback"), m_navModel->transModuleName(MODULES.at(3))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("deepin-image-viewer"), m_navModel->transModuleName(MODULES.at(4))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("deepin-launcher"), m_navModel->transModuleName(MODULES.at(5))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("deepin-movie"), m_navModel->transModuleName(MODULES.at(6))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("deepin-music"), m_navModel->transModuleName(MODULES.at(7))));
//    m_navModel->appendRow(new QStandardItem(QIcon::fromTheme("deepin-terminal"), m_navModel->transModuleName(MODULES.at(8))));

    m_navView->setModel(m_navModel);

    connect(m_navView, &NavWinView::clicked, this, &MainWindow::onItemClieck);
}

void MainWindow::pushWidget(QWidget *widget)
{
    if (m_contentStack.isEmpty()) {
        m_navView->setViewMode(QListView::ListMode);
        m_navView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        m_rightView->show();
    } else {
        m_contentStack.top()->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentStack.push(widget);
    m_rightContentLayout->addWidget(widget);
}

void MainWindow::popWidget()
{
    QWidget *w = m_contentStack.pop();

    m_rightContentLayout->removeWidget(w);
    w->setParent(nullptr);
    w->deleteLater();

    if (m_contentStack.isEmpty()) {
        m_navView->setViewMode(QListView::IconMode);
        m_navView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_rightView->hide();
    } else {
        m_contentStack.top()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

void MainWindow::onItemClieck(const QModelIndex &index)
{
    qDebug() << " index.column() : " << index.column() << "index.row() : " << index.row();

    QWidget *w = new QWidget(this);

    if (m_contentStack.size() > 0) {
        w->setStyleSheet("background: red");
    } else {
        w->setStyleSheet("background: blue");
    }

    w->setMinimumWidth(100);

    QPushButton *button = new QPushButton("pop", w);
    connect(button, &QPushButton::clicked, this, &MainWindow::popWidget);
    pushWidget(w);
}
