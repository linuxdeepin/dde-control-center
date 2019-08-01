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

#include "powerwidget.h"
#include "modules/power/powermodel.h"
#include <QTimer>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

PowerWidget::PowerWidget(QWidget *parent)
    : QWidget(parent)
    , layout(new QVBoxLayout)
    , m_listview(new QListView)
    , m_model(nullptr)
{

}

PowerWidget::~PowerWidget()
{

}

void PowerWidget::initialize()
{
    m_listview->setViewMode(QListView::ListMode);
    m_listview->setResizeMode(QListView::Adjust);
    m_listview->setSpacing(10);

    m_listmodel = new QStandardItemModel(this);
    m_listmodel->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), "通用"));
    m_listmodel->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), "Plugged In")),
          m_listmodel->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), "On Battery")),
          m_listview->setModel(m_listmodel);

    connect(m_listview, &QListView::pressed, this, &PowerWidget::onItemClieck);
    connect(this, &PowerWidget::requestRemoveBattery, this, [this](bool state) {
        if (!state) {
            m_listmodel->removeRow(2);
        }
    });

    layout->addWidget(m_listview);
    this->setLayout(layout);

    m_defaultIndex = m_listmodel->index(0, power_default_index);
}

void PowerWidget::setModel(const PowerModel *model)
{
    m_model = model;
}

void PowerWidget::requestDefaultWidget()
{
    QTimer::singleShot(0, this, [this] {
        m_listview->setCurrentIndex(m_defaultIndex);
        m_listview->pressed(m_defaultIndex);
    });
}

void PowerWidget::onItemClieck(const QModelIndex &index)
{
    Q_EMIT requestPushWidget(index.row());
}
