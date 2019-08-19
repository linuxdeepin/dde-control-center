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
#include "window/utils.h"
#include "modules/power/powermodel.h"

#include <DStyleOption>

#include <QTimer>

using namespace dcc::power;
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

void PowerWidget::initialize(bool hasBattery)
{
    m_listmodel = new QStandardItemModel(m_listview);

    QStringList menuList;
    menuList << tr("General") << tr("Plugged In") << tr("On Battery");

    QStandardItem *powerItem = nullptr;
    for (auto strMenuItem : menuList) {
        powerItem = new QStandardItem(strMenuItem);
        powerItem->setData(Dtk::RoundedBackground, Dtk::BackgroundTypeRole);
        powerItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_listmodel->appendRow(powerItem);
    }

    m_listview->setFrameShape(QFrame::NoFrame);
    m_listview->setModel(m_listmodel);
    m_listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listview->setCurrentIndex(m_listmodel->index(0, 0));
    m_listview->setRowHidden(2, !hasBattery);

    m_listmodel = new QStandardItemModel(this);
    m_listmodel->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), tr("General")));
    m_listmodel->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), tr("Plugged In"))),
          m_listmodel->appendRow(new QStandardItem(QIcon::fromTheme("dde-introduction"), tr("On Battery"))),
          m_listview->setModel(m_listmodel);

    connect(m_listview, &QListView::pressed, this, &PowerWidget::onItemClieck);
    connect(this, &PowerWidget::requestRemoveBattery, this, [this](bool state) {
        m_listview->setRowHidden(2, !state);
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
