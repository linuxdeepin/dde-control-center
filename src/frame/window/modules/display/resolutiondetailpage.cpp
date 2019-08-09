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
#include <QLabel>

using namespace dcc::widgets;
using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->addSpacing(10);
    setLayout(m_mainLayout);
}

void ResolutionDetailPage::setModel(DisplayModel *model)
{
    m_model = model;

    initResoList();
}

void ResolutionDetailPage::refreshCurrentResolution(const Resolution &reso, int listIdx)
{
    if (!m_model)
        return;

    const Monitor *mon = m_model->monitorList().first();
    if (!mon)
        return;

    auto list = mon->modeList();
    int idx = list.indexOf(reso);
    auto modelIdx = m_resoList[listIdx]->model()->index(idx, 0);
    m_resoList[listIdx]->setCurrentIndex(modelIdx);
}

void ResolutionDetailPage::initResoList()
{
    auto monitors = m_model->monitorList();
    const auto curMode = monitors.first()->currentMode();

    qDeleteAll(m_resoList);
    m_resoList.clear();

    for (int idx = 0; idx < monitors.size(); ++idx) {
        if (monitors.size() > 1) {
            m_mainLayout->addSpacing(10);
            QLabel *label = new QLabel(monitors.at(idx)->name());
            label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            m_mainLayout->addWidget(label);
        }

        QListView *rlist = new QListView();
        auto itemModel = new QStandardItemModel(this);
        rlist->setModel(itemModel);

        bool first = true;
        QStandardItem *curIdx{nullptr};
        const auto &modes = monitors[idx]->modeList();
        for (auto m : modes) {
            const QString res = QString("%1x%2").arg(m.width()).arg(m.height());
            QStandardItem *item = new QStandardItem();

            if (first) {
                first = false;
                item->setText(QString("%1 (%2)").arg(res).arg(tr("Recommended")));
            } else {
                item->setText(res);
            }

            if (curMode == m)
                curIdx = item;
            itemModel->appendRow(item);
        }

        if (nullptr != curIdx)
            rlist->setCurrentIndex(curIdx->index());

        connect(rlist, &QListView::clicked, [ = ](QModelIndex modelIdx) {
            Q_EMIT requestSetResolution(monitors.at(idx), modes[modelIdx.row()].id());
        });
        connect(monitors.at(idx), &Monitor::currentModeChanged, this,
        [ = ](const Resolution & resolution) {
            refreshCurrentResolution(resolution, idx);
        });

        m_mainLayout->addWidget(rlist);
        m_mainLayout->addSpacing(20);
        m_resoList.append(rlist);
    }

}
