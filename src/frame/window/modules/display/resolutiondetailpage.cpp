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
#include "widgets/translucentframe.h"
#include "window/utils.h"

#include <DStyleOption>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListView>
#include <QLabel>

using namespace dcc::widgets;
using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : dcc::ContentWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(ThirdPageContentsMargins);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(m_mainLayout);

    layout()->setMargin(0);
    setContent(centralWidget);
}

void ResolutionDetailPage::setModel(DisplayModel *model)
{
    m_model = model;

    connect(m_model, &DisplayModel::monitorListChanged,
            this, &ResolutionDetailPage::initResoList);

    auto monitors = m_model->monitorList();
    if(!monitors.size())
        return;

    connect(monitors.first(), &Monitor::modelListChanged,
            this, &ResolutionDetailPage::initResoList);
    initResoList();
}

void ResolutionDetailPage::initResoList()
{
    if (m_resoList)
        m_resoList->deleteLater();

    auto monitors = m_model->monitorList();
    if(!monitors.size()) {
        return;
    } else {
        disconnect(monitors.first(), &Monitor::modelListChanged,
                    this, &ResolutionDetailPage::initResoList);

        connect(monitors.first(), &Monitor::modelListChanged,
                this, &ResolutionDetailPage::initResoList);
    }

    auto moni = monitors.first();

    DListView *rlist = new DListView();
    rlist->setAutoScroll(false);
    rlist->setFrameShape(QFrame::NoFrame);
    rlist->setEditTriggers(DListView::NoEditTriggers);
    rlist->setSelectionMode(DListView::NoSelection);

    auto itemModel = new QStandardItemModel(this);
    rlist->setModel(itemModel);

    bool first = true;
    DStandardItem *curIdx{nullptr};
    const auto &modes = moni->modeList();
    Resolution prevM;
    for (auto m : modes) {
        if (Monitor::isSameResolution(m, prevM)) {
            continue;
        }
        prevM = m;
        const QString res = QString("%1x%2").arg(m.width()).arg(m.height());

        DStandardItem *item = new DStandardItem();
        if (first) {
            first = false;
            //~ contents_path /display/Resolution
            item->setText(QString("%1 (%2)").arg(res).arg(tr("Recommended")));
        } else {
            item->setText(res);
        }

        item->setData(QVariant(m.id()), IdRole);
        item->setData(QVariant(m.width()), WidthRole);
        item->setData(QVariant(m.height()), HeightRole);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        itemModel->appendRow(item);

        if (Monitor::isSameResolution(m, moni->currentMode()))
            curIdx = item;
    }

    if (nullptr != curIdx) {
        itemModel->setData(curIdx->index(), Qt::CheckState::Checked, Qt::CheckStateRole);
    }

    connect(rlist, &DListView::clicked, this, [ = ](const QModelIndex & idx) {
        if (itemModel->data(idx, Qt::CheckStateRole) == Qt::CheckState::Checked)
            return ;

        requestSetResolution(moni, itemModel->itemData(idx)[IdRole].toInt());
    });
    connect(moni, &Monitor::currentModeChanged, this, [ = ](const Resolution & r) {
        for(int i = 0; i < itemModel->rowCount(); ++i) {
            auto tItem = itemModel->item(i);
            auto w = tItem->data(WidthRole).toInt();
            auto h = tItem->data(HeightRole).toInt();
            if(w == r.width() && h == r.height()) {
                tItem->setData(Qt::CheckState::Checked, Qt::CheckStateRole);
            } else {
                tItem->setData(Qt::CheckState::Unchecked, Qt::CheckStateRole);
            }
        }
    });

    m_mainLayout->addWidget(rlist, 1);
    m_resoList = rlist;
}
