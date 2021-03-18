/*
 * Copyright (C) 2020 Deepin Technology Co., Ltd.
 *
 * Author:     xiaoyaobing <xiaoyaobing@uniontech.com>
 *
 * Maintainer: xiaoyaobing <xiaoyaobing@uniontech.com>
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

#include "privacywidget.h"

#include <DListView>
#include <DStandardItem>

#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

PrivacyWidget::PrivacyWidget(QWidget *parent)
    : QWidget(parent)
    , m_privacyItemView(new DListView(this))
    , m_privacyModel(new QStandardItemModel(this))
    , m_centralLayout(new QVBoxLayout)
{
    initUI();
}

void PrivacyWidget::initUI()
{
    m_privacyItemView->setModel(m_privacyModel);
    m_privacyItemView->setResizeMode(QListView::Adjust);
    m_privacyItemView->setEditTriggers(QAbstractItemView:: NoEditTriggers);
    m_privacyItemView->setAutoScroll(false);
    m_privacyItemView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_privacyItemView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_privacyItemView->setViewportMargins(QMargins(0, 0, 10, 0));

    DStandardItem *locationItem = new DStandardItem(QIcon(":/res/location.svg"), tr("Location service"));
    DStandardItem *photoItem = new DStandardItem(QIcon(":/res/photo.svg"), tr("Photo"));
    DStandardItem *bluetoothItem = new DStandardItem(QIcon(":/res/bluetooth.svg"), tr("Bluetooth"));
    DStandardItem *microphoneItem = new DStandardItem(QIcon(":/res/Microphone.svg"), tr("Microphone"));

    m_privacyModel->appendRow(locationItem);
    m_privacyModel->appendRow(photoItem);
    m_privacyModel->appendRow(bluetoothItem);
    m_privacyModel->appendRow(microphoneItem);

    m_centralLayout->addWidget(m_privacyItemView);
    m_centralLayout->setContentsMargins(10,10,0,0);
    setLayout(m_centralLayout);

    connect(m_privacyItemView, &DListView::clicked, this, &PrivacyWidget::onPrivacyItemViewClicked);
    connect(m_privacyItemView, &DListView::activated, m_privacyItemView, &DListView::clicked);

    m_lastIndex = m_privacyItemView->model()->index(0, 0);
    m_privacyItemView->setCurrentIndex(m_lastIndex);
}

void PrivacyWidget::onPrivacyItemViewClicked(const QModelIndex &index)
{

    if (index == m_lastIndex) return;

    m_lastIndex = index;
    Q_EMIT requestShowPrivacyView(index.row());
}
