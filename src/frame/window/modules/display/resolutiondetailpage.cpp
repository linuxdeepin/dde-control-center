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
#include <QStandardItemModel>
#include <QStandardItem>
#include <QLabel>

using namespace dcc::widgets;
using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

ResolutionDetailPage::ResolutionDetailPage(QWidget *parent)
    : dcc::ContentWidget(parent)
    , m_resoListLayout(new QVBoxLayout)
{
    auto mainLayout(new QVBoxLayout);
    mainLayout->setMargin(0);
    mainLayout->addSpacing(10);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(mainLayout);
    mainLayout->addLayout(m_resoListLayout);

    QHBoxLayout *hlayout = new QHBoxLayout;
    m_cancelBtn = new QPushButton(tr("cancel"));
    m_cancelBtn->setEnabled(false);
    connect(m_cancelBtn, &QPushButton::clicked, this, [ this ] {
        Q_EMIT this->requestReset();

        m_saveBtn->setEnabled(false);
        m_cancelBtn->setEnabled(false);
    });
    hlayout->addWidget(m_cancelBtn);

    m_saveBtn = new QPushButton(tr("save"));
    m_saveBtn->setEnabled(false);
    connect(m_saveBtn, &QPushButton::clicked, this, [ this ] {
        Q_EMIT this->requestSave();

        m_saveBtn->setEnabled(false);
        m_cancelBtn->setEnabled(false);
        m_initialIdxs = m_curIdxs;
    });
    hlayout->addWidget(m_saveBtn);

    mainLayout->addStretch(1);
    mainLayout->addLayout(hlayout);

    setContent(centralWidget);
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

    if (listIdx >= m_curIdxs.size())
        return;

    auto list = m_model->monitorList()[listIdx]->modeList();
    int idx = list.indexOf(reso);
    auto model = m_resoList[listIdx]->model();
    auto modelIdx = model->index(idx, 0);
    checkedChange(modelIdx, m_resoList[listIdx]);
}

void ResolutionDetailPage::initResoList()
{
    auto monitors = m_model->monitorList();

    qDeleteAll(m_resoList);
    m_resoList.clear();
    m_curIdxs.clear();

    for (int idx = 0; idx < monitors.size(); ++idx) {
        if (monitors.size() > 1) {
            m_resoListLayout->addSpacing(10);
            QLabel *label = new QLabel(monitors.at(idx)->name());
            label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            m_resoListLayout->addWidget(label);
        }

        QListView *rlist = new QListView();
        rlist->setAutoScroll(false);
        rlist->setFrameShape(QFrame::NoFrame);
        rlist->setSpacing(5);
        rlist->setSelectionMode(QListView::NoSelection);

        auto itemModel = new QStandardItemModel(this);
        rlist->setModel(itemModel);

        bool first = true;
        QStandardItem *curIdx{nullptr};
        const auto &modes = monitors[idx]->modeList();
        const auto curMode = monitors[idx]->currentMode();
        for (auto m : modes) {
            const QString res = QString("%1x%2").arg(m.width()).arg(m.height());
            QStandardItem *item = new QStandardItem();

            if (first) {
                first = false;
                item->setText(QString("%1 (%2)").arg(res).arg(tr("Recommended")));
            } else {
                item->setText(res);
            }
            item->setCheckable(true);
            item->setData(Dtk::RoundedBackground, Dtk::BackgroundTypeRole);
            item->setData(VListViewItemMargin, Dtk::MarginsRole);

            if (curMode == m)
                curIdx = item;
            itemModel->appendRow(item);
        }

        if (nullptr != curIdx) {
            itemModel->setData(curIdx->index(), Qt::CheckState::Checked, Qt::CheckStateRole);
            m_curIdxs.push_back(curIdx->index());
            m_initialIdxs.push_back(curIdx->index());
        } else {
            m_curIdxs.push_back(QModelIndex());
        }

        connect(rlist, &QListView::clicked, this, &ResolutionDetailPage::onListClick);
        connect(monitors.at(idx), &Monitor::currentModeChanged, this,
        [ = ](const Resolution & resolution) {
            refreshCurrentResolution(resolution, idx);
        });
        m_resoListLayout->addWidget(rlist);
        m_resoListLayout->addSpacing(20);
        m_resoList.append(rlist);
    }

}

void ResolutionDetailPage::checkedChange(const QModelIndex &idx, QListView *list)
{
    int listIdx = m_resoList.indexOf(list);

    if (idx == m_curIdxs[listIdx])
        return;

    auto listModel = m_resoList[listIdx]->model();
    listModel->setData(m_curIdxs[listIdx], Qt::CheckState::Unchecked, Qt::CheckStateRole);
    listModel->setData(idx, Qt::CheckState::Checked, Qt::CheckStateRole);
    m_curIdxs[listIdx] = idx;

    if (idx != m_initialIdxs[listIdx]) {
        m_saveBtn->setEnabled(true);
        m_cancelBtn->setEnabled(true);
        return;
    }

    Q_ASSERT(m_curIdxs.size() == m_initialIdxs.size());
    for (int i = 0; i < m_initialIdxs.size() ; ++i) {
        if (m_curIdxs[i] != m_initialIdxs[i]) {
            m_saveBtn->setEnabled(true);
            m_cancelBtn->setEnabled(true);
            return;
        }
    }

    m_saveBtn->setEnabled(false);
    m_cancelBtn->setEnabled(false);
}

void ResolutionDetailPage::onListClick(const QModelIndex &idx)
{
    auto list = qobject_cast<QListView *>(sender());
    if (!list)
        return;


    int listIdx = m_resoList.indexOf(list);
    if (idx == m_curIdxs[listIdx])
        return;

    auto moni = m_model->monitorList()[listIdx];
    Q_EMIT this->requestSetResolution(moni, moni->modeList()[idx.row()].id());
}
