/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#include "resolutiondetailpage.h"
#include "modules/display/displaymodel.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>

using namespace dcc::widgets;
using namespace dcc::display;
using namespace DCC_NAMESPACE::display;


ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : QWidget(parent),
      m_mainLayout(new QVBoxLayout),
      m_modeList(new QListView)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_modeList);
    setLayout(m_mainLayout);
}

void ResolutionDetailPage::setModel(DisplayModel *model)
{
    m_model = model;

    QStandardItemModel* itemModel {nullptr};
    if (itemModel = dynamic_cast<QStandardItemModel*>(m_modeList->model())) {
        itemModel->clear();
    }

    bool first = true;
    auto monitors = model->monitorList();
    auto modes = monitors.first()->modeList();
    const auto curMode = monitors.first()->currentMode();

    itemModel = new QStandardItemModel(this);
    QStandardItem *curIdx{nullptr};
    for (auto m : modes) {
        const QString res = QString::number(m.width()) + "×" + QString::number(m.height());
        QStandardItem *item = new QStandardItem();

        if (first) {
            first = false;
            item->setText(res + tr(" (Recommended)"));
        } else {
            item->setText(res);
        }

        if (curMode == m)
            curIdx = item;
        itemModel->appendRow(item);
    }
    m_modeList->setModel(itemModel);
    if (nullptr != curIdx)
        m_modeList->setCurrentIndex(curIdx->index());

    connect(m_modeList, &QListView::clicked, [ = ](QModelIndex idx) {
        Q_EMIT requestSetResolution(monitors.first(), modes[idx.row()].id());
    });
}
