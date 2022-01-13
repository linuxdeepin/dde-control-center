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

#include "mousewidget.h"
#include "window/utils.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"
#include "widgets/multiselectlistview.h"
#include <QVBoxLayout>
#include <QList>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::widgets;

MouseWidget::MouseWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("Mouse");
    m_contentLayout = new QVBoxLayout(this);
    m_mouseListView = new dcc::widgets::MultiSelectListView(this);
    m_mouseListView->setAccessibleName("List_mouselist");
    m_contentLayout->setMargin(0);
    m_contentLayout->addWidget(m_mouseListView);
    setLayout(m_contentLayout);
}

void MouseWidget::init(bool tpadExist, bool redPointExist)
{
    qDebug() << "tpadExist: " << tpadExist << "redPoint: " << redPointExist;
    m_listviewModel = new QStandardItemModel(m_mouseListView);
    QList<QPair<QIcon, QString>> menuIconText;
    menuIconText = {
        { QIcon::fromTheme("dcc_general_purpose"), tr("General")},
        //~ contents_path /mouse/Mouse
        { QIcon::fromTheme("dcc_mouse"), tr("Mouse")},
        //~ contents_path /mouse/Touchpad
        { QIcon::fromTheme("dcc_touchpad"), tr("Touchpad")},
        //~ contents_path /mouse/TrackPoint
        { QIcon::fromTheme("dcc_trackpoint"), tr("TrackPoint")}
    };
    DStandardItem *mouseItem = nullptr;
    for (auto it = menuIconText.cbegin(); it != menuIconText.cend(); ++it) {
        mouseItem = new DStandardItem(it->first, it->second);
        mouseItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        if (it->second == QString(tr("Mouse"))) {
            mouseItem->setAccessibleText("MOUSE_ITEM");
        }
        m_listviewModel->appendRow(mouseItem);
    }
    m_mouseListView->setAccessibleName("List_mousemenulist");
    m_mouseListView->setFrameShape(QFrame::NoFrame);
    m_mouseListView->setModel(m_listviewModel);
    m_mouseListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mouseListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_lastIndex = m_listviewModel->index(0, 0);
    m_mouseListView->setCurrentIndex(m_lastIndex);
    m_mouseListView->setRowHidden(2, !tpadExist);
    m_mouseListView->setRowHidden(3, !redPointExist);
    m_mouseListView->setViewportMargins(ScrollAreaMargins);
    m_mouseListView->setIconSize(ListViweIconSize);
    m_mouseListView->resetStatus(m_lastIndex);
    connect(m_mouseListView, &DListView::clicked, this, &MouseWidget::onItemClicked);
    connect(m_mouseListView, &DListView::activated, m_mouseListView, &QListView::clicked);
    connect(this, &MouseWidget::tpadExistChanged, this, [this](bool bExist) {
        m_mouseListView->setRowHidden(2, !bExist);
        qDebug() << "tpadExistChanged: " << bExist;
    });
    connect(this, &MouseWidget::redPointExistChanged, this, [this](bool bExist) {
        m_mouseListView->setRowHidden(3, !bExist);
        qDebug() << "redPointExistChanged: " << bExist;
    });
}

void MouseWidget::initSetting(const int settingIndex)
{
    m_mouseListView->clicked(m_listviewModel->index(settingIndex, 0));
}

void MouseWidget::onItemClicked(const QModelIndex &index)
{
    if (index == m_lastIndex || index.row() > m_mouseListView->count() - 1)
        return;

    if (index.row() < 0 || m_mouseListView->isRowHidden(index.row())) {
        m_mouseListView->resetStatus(m_lastIndex);
        return;
    }

    m_lastIndex = index;
    switch (m_lastIndex.row()) {
    case 0:
        Q_EMIT showGeneralSetting();
        break;
    case 1:
        Q_EMIT showMouseSetting();
        break;
    case 2:
        Q_EMIT showTouchpadSetting();
        break;
    case 3:
        Q_EMIT showTrackPointSetting();
        break;
    default:
        Q_EMIT showGeneralSetting();
        break;
    }

    m_mouseListView->setCurrentIndex(m_lastIndex);  
    m_mouseListView->resetStatus(m_lastIndex);
}
