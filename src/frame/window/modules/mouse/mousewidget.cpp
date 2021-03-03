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
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "modules/mouse/mousemodel.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"
#include "widgets/multiselectlistview.h"
#include "window/insertplugin.h"
#include "window/gsettingwatcher.h"
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
    m_menuIconText.push_back({ "dcc_general_purpose", tr("General"), QMetaMethod::fromSignal(&MouseWidget::showGeneralSetting)});
    //~ contents_path /mouse/Mouse
    m_menuIconText.push_back({ "dcc_mouse", tr("Mouse"), QMetaMethod::fromSignal(&MouseWidget::showMouseSetting)});
    //~ contents_path /mouse/Touchpad
    if (GSettingWatcher::instance()->getStatus("mouseTouchpad") != "Hidden")
        m_menuIconText.push_back({ "dcc_touchpad", tr("Touchpad"), QMetaMethod::fromSignal(&MouseWidget::showTouchpadSetting)});
    //~ contents_path /mouse/TrackPoint
    m_menuIconText.push_back({ "dcc_trackpoint", tr("TrackPoint"), QMetaMethod::fromSignal(&MouseWidget::showTrackPointSetting)});

    QList<DStandardItem *> mouseItems;
    for (auto it = m_menuIconText.cbegin(); it != m_menuIconText.cend(); ++it) {
        DStandardItem *mouseItem = new DStandardItem(QIcon::fromTheme(it->itemIcon), it->itemText);
        mouseItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        if (it->itemText == QString(tr("Mouse"))) {
            mouseItem->setAccessibleText("MOUSE_ITEM");
        }
        if (!tpadExist && it->itemText == QString(tr("Touchpad"))) {
            continue;
        }
        if (!redPointExist && it->itemText == QString(tr("TrackPoint"))) {
            continue;
        }
        m_listviewModel->appendRow(mouseItem);
        m_mouseItems << mouseItem;
    }

    if (InsertPlugin::instance()->needPushPlugin("mouse")) {
        InsertPlugin::instance()->pushPlugin(m_listviewModel, m_menuIconText);
    }

    m_mouseListView->setAccessibleName("List_mousemenulist");
    m_mouseListView->setFrameShape(QFrame::NoFrame);
    m_mouseListView->setModel(m_listviewModel);
    m_mouseListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mouseListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_lastIndex = m_listviewModel->index(0, 0);
    m_mouseListView->resetStatus(m_lastIndex);
    m_mouseListView->setViewportMargins(ScrollAreaMargins);
    m_mouseListView->setIconSize(ListViweIconSize);
    connect(m_mouseListView, &DListView::clicked, this, &MouseWidget::onItemClicked);
    connect(m_mouseListView, &DListView::activated, m_mouseListView, &QListView::clicked);
    connect(this, &MouseWidget::tpadExistChanged, this, [ = ](bool bExist) {
        if (bExist) {
                DStandardItem *mouseTouchpadItem = new DStandardItem(QIcon::fromTheme(m_menuIconText[2].itemIcon), m_menuIconText[2].itemText);
                mouseTouchpadItem->setData(VListViewItemMargin, Dtk::MarginsRole);

                m_listviewModel->insertRow(2, mouseTouchpadItem);
                m_mouseItems << mouseTouchpadItem;
        } else {
            DStandardItem *touchItem = nullptr;
            for (auto item : m_mouseItems) {
                if (item->text() == QString(tr("Touchpad"))) {
                    touchItem = item;
                    break;
                }
            }
            if (touchItem) {
                if (m_mouseListView->count() < 4) {
                    m_mouseListView->resetStatus(m_listviewModel->index(0, 0));
                }
                m_mouseListView->removeItem(m_listviewModel->indexFromItem(touchItem).row());
            }

        }
    });
    connect(this, &MouseWidget::redPointExistChanged, this, [ = ](bool bExist) {
        if (bExist) {
            DStandardItem *mouseTrackPointItem = new DStandardItem(QIcon::fromTheme(m_menuIconText[3].itemIcon), m_menuIconText[3].itemText);
            mouseTrackPointItem->setData(VListViewItemMargin, Dtk::MarginsRole);

            m_listviewModel->insertRow(3, mouseTrackPointItem);
            m_mouseItems << mouseTrackPointItem;
        } else {
            DStandardItem *trackPointItem = nullptr;
            for (auto item : m_mouseItems) {
                if (item->text() == QString(tr("TrackPoint"))) {
                    trackPointItem = item;
                    break;
                }
            }
            if (trackPointItem)
                m_mouseListView->removeItem(m_listviewModel->indexFromItem(trackPointItem).row());
        }
    });
}

void MouseWidget::initSetting(const int settingIndex)
{
    m_mouseListView->setCurrentIndex(m_listviewModel->index(settingIndex, 0));
    m_mouseListView->clicked(m_listviewModel->index(settingIndex, 0));
}

void MouseWidget::setDefaultWidget()
{
    m_menuIconText[0].itemSignal.invoke(m_menuIconText[0].pulgin ? m_menuIconText[0].pulgin : this);
}

void MouseWidget::onItemClicked(const QModelIndex &index)
{
    if (m_lastIndex == index) return;

    m_lastIndex = index;
    m_menuIconText[index.row()].itemSignal.invoke(m_menuIconText[index.row()].pulgin ? m_menuIconText[index.row()].pulgin : this);
    m_mouseListView->resetStatus(index);
}
