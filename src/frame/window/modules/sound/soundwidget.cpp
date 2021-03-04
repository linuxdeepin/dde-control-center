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

#include "soundwidget.h"

#include "modules/sound/soundworker.h"
#include "widgets/multiselectlistview.h"
#include "window/gsettingwatcher.h"
#include "window/utils.h"

#include <DStyleOption>

#include <QListView>
#include <QMargins>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE::sound;

SoundWidget::SoundWidget(QWidget *parent)
    : QWidget(parent)
    , m_listView(new dcc::widgets::MultiSelectListView)
    , m_itemModel(new QStandardItemModel(this))
{
    setObjectName("Sound");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    initUi();
    initMembers();
    initConnections();
}

SoundWidget::~SoundWidget()
{
}

void SoundWidget::initUi()
{
    QVBoxLayout *soundLayout = new QVBoxLayout(this);
    soundLayout->setContentsMargins(0, 0, 0, 0);

    m_listView->setAccessibleName("List_soundmenulist");
    m_listView->setEditTriggers(DListView::NoEditTriggers);
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setViewportMargins(ScrollAreaMargins);
    m_listView->setIconSize(ListViweIconSize);
    m_listView->setModel(m_itemModel);
    m_listView->setCurrentIndex(m_itemModel->index(0, 0));
    m_currentIdx = m_listView->currentIndex();
    m_listView->resetStatus(m_currentIdx);
    soundLayout->addWidget(m_listView);
}

void SoundWidget::initMembers()
{
    //~ contents_path /sound/Speaker
    m_menuMethod.append({"dcc_speaker", tr("Output"), QMetaMethod::fromSignal(&SoundWidget::requsetSpeakerPage), nullptr, "soundOutput"});
    //~ contents_path /sound/Microphone
    m_menuMethod.append({"dcc_noun", tr("Input"), QMetaMethod::fromSignal(&SoundWidget::requestMicrophonePage), nullptr, "soundInput"});
    //~ contents_path /sound/Sound Effects
    m_menuMethod.append({"dcc_sound_effect", tr("Sound Effects"), QMetaMethod::fromSignal(&SoundWidget::requsetSoundEffectsPage), nullptr, "soundEffects"});

    for (auto mm : m_menuMethod) {
        DStandardItem *item = new DStandardItem(mm.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setIcon(QIcon::fromTheme(mm.itemIcon));
        m_itemModel->appendRow(item);
        GSettingWatcher::instance()->bind(mm.gsettingsName, m_listView, item);
    }

    if (InsertPlugin::instance()->needPushPlugin("sound")) {
        InsertPlugin::instance()->pushPlugin(m_itemModel, m_menuMethod);
    }
}

void SoundWidget::initConnections()
{
    connect(m_listView, &QListView::clicked, [=](const QModelIndex &idx) {
        if (idx == m_currentIdx)
            return;

        m_currentIdx = idx;
        m_menuMethod[idx.row()].itemSignal.invoke(m_menuMethod[idx.row()].pulgin ? m_menuMethod[idx.row()].pulgin : this);
        m_listView->resetStatus(idx);
    });
    connect(m_listView, &DListView::activated, m_listView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row) {
        bool isAllHidden = true;
        for (int i = 0; i < m_itemModel->rowCount(); i++) {
            if (!m_listView->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_listView->selectionModel()->selectedRows().size() > 0) {
            int index = m_listView->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHidden) {
            m_currentIdx = QModelIndex();
            m_listView->clearSelection();
        }
    });
}


int SoundWidget::showPath(const QString &path)
{
    for (int i = 0; i < m_menuMethod.size(); ++i) {
        auto menu = m_menuMethod[i];
        if (tr(path.toStdString().c_str()) == menu.itemText) {
            menu.itemSignal.invoke(this);
            m_currentIdx = m_listView->model()->index(i, 0);
            m_listView->setCurrentIndex(m_currentIdx);
            return 0;
        }
    }

    return -1;
}

void SoundWidget::showDefaultWidget()
{
    for (int i = 0; i < m_listView->model()->rowCount(); i++) {
        if (!m_listView->isRowHidden(i)) {
            m_listView->activated(m_listView->model()->index(i, 0));
            break;
        }
    }
}
