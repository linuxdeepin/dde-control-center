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

#include "keyboardwidget.h"
#include "window/utils.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/multiselectlistview.h"
#include <QVBoxLayout>
#include <QList>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc::widgets;
KeyboardWidget::KeyboardWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("Mouse");
    m_contentLayout = new QVBoxLayout(this);
    m_contentLayout->setMargin(0);
    m_keyboardListView = new dcc::widgets::MultiSelectListView(this);
    m_contentLayout->addWidget(m_keyboardListView);
    init();
    setLayout(m_contentLayout);
}

void KeyboardWidget::init()
{
    m_listviewModel = new QStandardItemModel(m_keyboardListView);
    QList<QPair<QIcon, QString>> menuIconText;
    menuIconText = {
        { QIcon::fromTheme("dcc_general_purpose"), tr("General")},
        //~ contents_path /keyboard/Keyboard Layout
        { QIcon::fromTheme("dcc_keyboard"), tr("Keyboard Layout")},
        //~ contents_path /keyboard/System Language
        { QIcon::fromTheme("dcc_language"), tr("System Language")},
        //~ contents_path /keyboard/Shortcuts
        { QIcon::fromTheme("dcc_hot_key"), tr("Shortcuts")}
    };
    DStandardItem *keyboardItem = nullptr;
    for (auto it = menuIconText.cbegin(); it != menuIconText.cend(); ++it) {
        keyboardItem = new DStandardItem(it->first, it->second);
        keyboardItem->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_listviewModel->appendRow(keyboardItem);
    }
    m_keyboardListView->setAccessibleName("List_keyboardlist");
    m_keyboardListView->setFrameShape(QFrame::NoFrame);
    m_keyboardListView->setModel(m_listviewModel);
    m_keyboardListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_keyboardListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_keyboardListView->setCurrentIndex(m_listviewModel->index(0, 0));
    m_keyboardListView->setViewportMargins(ScrollAreaMargins);
    m_keyboardListView->setIconSize(ListViweIconSize);
    m_lastIndex = m_keyboardListView->currentIndex();
    connect(m_keyboardListView, &DListView::clicked, this, &KeyboardWidget::onItemClick);
    connect(m_keyboardListView, &DListView::activated, m_keyboardListView, &QListView::clicked);
}

void KeyboardWidget::initSetting(const int settingIndex)
{
    m_keyboardListView->setCurrentIndex(m_listviewModel->index(settingIndex, 0));
    m_keyboardListView->clicked(m_listviewModel->index(settingIndex, 0));
}

void KeyboardWidget::onItemClick(const QModelIndex &index)
{
    if (index == m_lastIndex) {
        return;
    }
    switch (index.row()) {
    case 0:
        Q_EMIT showGeneralSetting();
        break;
    case 1:
        Q_EMIT showKBLayoutSetting();
        break;
    case 2:
        Q_EMIT showSystemLanguageSetting();
        break;
    case 3:
        if (index.row() != m_previewRow)
            Q_EMIT showShortCutSetting();
        break;
    default:
        Q_EMIT showGeneralSetting();
        break;
    }

    m_previewRow = index.row();
    m_lastIndex = index;
    m_keyboardListView->resetStatus(index);
}
