// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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

MouseWidget::~MouseWidget()
{

}

void MouseWidget::init(bool tpadExist, bool redPointExist)
{
    qDebug() << "tpadExist: " << tpadExist << "redPoint: " << redPointExist;
    m_listviewModel = new QStandardItemModel(m_mouseListView);
    m_mouseListView->setAccessibleName("List_mousemenulist");
    m_mouseListView->setFrameShape(QFrame::NoFrame);
    m_mouseListView->setModel(m_listviewModel);
    m_mouseListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mouseListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_mouseListView->resetStatus(m_lastIndex);
    m_mouseListView->setViewportMargins(ScrollAreaMargins);
    m_mouseListView->setIconSize(ListViweIconSize);

    m_menuIconText.push_back({ "dcc_general_purpose", tr("General"), QMetaMethod::fromSignal(&MouseWidget::showGeneralSetting), nullptr, "mouseGeneral"});
    m_menuIconText.push_back({ "dcc_mouse", tr("Mouse"), QMetaMethod::fromSignal(&MouseWidget::showMouseSetting), nullptr, "mouseMouse"});
    if (GSettingWatcher::instance()->getStatus("mouseTouchpad") != "Hidden")
        m_menuIconText.push_back({ "dcc_touchpad", tr("Touchpad"), QMetaMethod::fromSignal(&MouseWidget::showTouchpadSetting), nullptr, "mouseTouch"});
    m_menuIconText.push_back({ "dcc_trackpoint", tr("TrackPoint"), QMetaMethod::fromSignal(&MouseWidget::showTrackPointSetting), nullptr, "mouseTrackpoint"});

    for (auto it = m_menuIconText.cbegin(); it != m_menuIconText.cend(); ++it) {
        if (!tpadExist && it->itemText == QString(tr("Touchpad"))) {
            continue;
        }
        if (!redPointExist && it->itemText == QString(tr("TrackPoint"))) {
            continue;
        }

        DStandardItem *mouseItem = new DStandardItem(QIcon::fromTheme(it->itemIcon), it->itemText);
        mouseItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        if (it->itemText == QString(tr("Mouse"))) {
            mouseItem->setAccessibleText("MOUSE_ITEM");
        }
        m_listviewModel->appendRow(mouseItem);
        m_mouseItems << mouseItem;
        GSettingWatcher::instance()->bind(it->gsettingsName, m_mouseListView, mouseItem);
    }

    if (InsertPlugin::instance()->updatePluginInfo("mouse")) {
        InsertPlugin::instance()->pushPlugin(m_listviewModel, m_menuIconText);
    }

    connect(m_mouseListView, &DListView::clicked, this, &MouseWidget::onItemClicked);
    connect(m_mouseListView, &DListView::activated, m_mouseListView, &QListView::clicked);
    connect(this, &MouseWidget::tpadExistChanged, this, [ = ](bool bExist) {
        if (bExist) {
                DStandardItem *mouseTouchpadItem = new DStandardItem(QIcon::fromTheme(m_menuIconText[2].itemIcon), m_menuIconText[2].itemText);
                mouseTouchpadItem->setData(VListViewItemMargin, Dtk::MarginsRole);

                m_listviewModel->insertRow(2, mouseTouchpadItem);
                m_mouseItems << mouseTouchpadItem;
                GSettingWatcher::instance()->bind("mouseTouch", m_mouseListView, mouseTouchpadItem);
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
                GSettingWatcher::instance()->erase("mouseTouch");
            }
        }
    });
    connect(this, &MouseWidget::redPointExistChanged, this, [ = ](bool bExist) {
        if (bExist) {
            DStandardItem *mouseTrackPointItem = new DStandardItem(QIcon::fromTheme(m_menuIconText[3].itemIcon), m_menuIconText[3].itemText);
            mouseTrackPointItem->setData(VListViewItemMargin, Dtk::MarginsRole);

            m_listviewModel->insertRow(3, mouseTrackPointItem);
            m_mouseItems << mouseTrackPointItem;
            GSettingWatcher::instance()->bind("mouseTrackpoint", m_mouseListView, mouseTrackPointItem);
        } else {
            DStandardItem *trackPointItem = nullptr;
            for (auto item : m_mouseItems) {
                if (item->text() == QString(tr("TrackPoint"))) {
                    trackPointItem = item;
                    break;
                }
            }
            if (trackPointItem) {
                m_mouseListView->removeItem(m_listviewModel->indexFromItem(trackPointItem).row());
                GSettingWatcher::instance()->erase("mouseTrackpoint");
            }
        }
    });

    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [ = ](const int row, const QString & name) {
        //不是本模块配置不响应
        if (!configContent(name))
            return ;
        bool isAllHidden = true;
        for (int i = 0; i < m_mouseListView->model()->rowCount(); i++) {
            if (!m_mouseListView->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_mouseListView->selectionModel()->selectedRows().size() > 0) {
            const int index = m_mouseListView->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHidden) {
            m_lastIndex = QModelIndex();
            m_mouseListView->clearSelection();
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
    for(int i = 0; i < m_mouseListView->model()->rowCount(); i++) {
        if (!m_mouseListView->isRowHidden(i)) {
            m_mouseListView->activated(m_mouseListView->model()->index(i, 0));
            break;
        }
    }
}

void MouseWidget::onItemClicked(const QModelIndex &index)
{
    if (m_lastIndex == index) return;

    m_lastIndex = index;
    m_menuIconText[index.row()].itemSignal.invoke(m_menuIconText[index.row()].plugin ? m_menuIconText[index.row()].plugin : this);
    m_mouseListView->resetStatus(index);
}

bool MouseWidget::configContent(const QString &configName)
{
    for (auto m : m_menuIconText) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}
