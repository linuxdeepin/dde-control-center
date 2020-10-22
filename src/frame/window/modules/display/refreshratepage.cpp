/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "refreshratepage.h"
#include "window/utils.h"

#include "modules/display/displaymodel.h"

#include <DListView>

#include <QVBoxLayout>

using namespace DCC_NAMESPACE::display;
using namespace dcc::display;
DWIDGET_USE_NAMESPACE

RefreshRatePage::RefreshRatePage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
{
    m_layout->setMargin(0);
    m_layout->setContentsMargins(ThirdPageContentsMargins);

    setLayout(m_layout);
}

void RefreshRatePage::setModel(DisplayModel *model)
{
    m_model = model;
    qDebug() << model->monitorList().size();
    m_monitor = model->monitorList().first();

    initRateList();
}

void RefreshRatePage::initRateList()
{
    DListView *list = new DListView();
    list->setAccessibleName("List_refreshratelist");
    list->setAutoScroll(false);
    list->setFrameShape(QFrame::NoFrame);
    list->setSelectionMode(DListView::NoSelection);
    list->setEditTriggers(DListView::NoEditTriggers);
    auto listModel = new QStandardItemModel(this);
    list->setModel(listModel);

    auto moni = m_model->monitorList().first();
    QList<double> rateList;
    bool isFirst = true;
    qDebug() << "moni->modeList size:" << moni->modeList().size();
    qDebug() << "moni current mode :" << moni->currentMode().width()
             << "x" << moni->currentMode().height();
    bool hasRecommend = false;
    for (auto m : moni->modeList()) {
        if (!Monitor::isSameResolution(m, moni->currentMode()))
            continue;

        qDebug() << "get same resolution" << m.width()
                 << "x" << m.height();
        auto trate = m.rate();
        DStandardItem *item = new DStandardItem;
        listModel->appendRow(item);

        auto tstr = QString::number(trate, 'g', 4) + tr("Hz");
        if (Monitor::isSameResolution(m, moni->bestMode())) {
            if (Monitor::isSameRatefresh(m, moni->bestMode())) {
                tstr += QString(" (%1)").arg(tr("Recommended"));
                isFirst = false;
                hasRecommend = true;
            }
        } else if (isFirst) {
            tstr += QString(" (%1)").arg(tr("Recommended"));
            isFirst = false;
            hasRecommend = true;
        }

        if (fabs(trate - moni->currentMode().rate()) < 0.000001) {
            item->setCheckState(Qt::CheckState::Checked);
        } else {
            item->setCheckState(Qt::CheckState::Unchecked);
        }
        item->setData(QVariant(m.id()), Qt::WhatsThisPropertyRole);
        item->setText(tstr);
    }

    if (!hasRecommend) {
        qDebug() << "BestMode provided by server is not in the ModeList";
    }

    connect(list, &DListView::clicked, this, [ = ](const QModelIndex & idx) {
        if (listModel->data(idx, Qt::CheckStateRole) == Qt::CheckState::Checked)
            return ;

        this->requestSetResolution(m_monitor, listModel->data(idx, Qt::WhatsThisPropertyRole).toInt());
    });

    connect(m_monitor, &Monitor::currentModeChanged, this, [ = ](const Resolution & r) {
        for (int i = 0; i < listModel->rowCount(); ++i) {
            auto tItem = listModel->item(i);
            if (tItem->data(Qt::WhatsThisPropertyRole).toInt() == r.id()) {
                tItem->setData(Qt::CheckState::Checked, Qt::CheckStateRole);
            } else {
                tItem->setData(Qt::CheckState::Unchecked, Qt::CheckStateRole);
            }
        }
    });

    m_layout->addWidget(list);
}
