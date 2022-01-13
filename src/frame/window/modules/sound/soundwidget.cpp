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
    m_itemList.append({"dcc_speaker", tr("Output"), QMetaMethod::fromSignal(&SoundWidget::requsetSpeakerPage), nullptr, "soundOutput"});
    m_itemList.append({"dcc_noun", tr("Input"), QMetaMethod::fromSignal(&SoundWidget::requestMicrophonePage), nullptr, "soundInput"});
    m_itemList.append({"dcc_sound_effect", tr("Sound Effects"), QMetaMethod::fromSignal(&SoundWidget::requsetSoundEffectsPage), nullptr, "soundEffects"});
    m_itemList.append({"dcc_device_mange", tr("Devices"), QMetaMethod::fromSignal(&SoundWidget::requsetDeviceManagesPage), nullptr, "deviceManage"});

    for (auto mm : m_itemList) {
        DStandardItem *item = new DStandardItem(mm.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        item->setIcon(QIcon::fromTheme(mm.itemIcon));
        m_itemModel->appendRow(item);
        GSettingWatcher::instance()->bind(mm.gsettingsName, m_listView, item);
    }

    if (InsertPlugin::instance()->updatePluginInfo("sound")) {
        InsertPlugin::instance()->pushPlugin(m_itemModel, m_itemList);
    }
}

void SoundWidget::initConnections()
{
    connect(m_listView, &QListView::clicked, [=](const QModelIndex &idx) {
        if (idx == m_currentIdx)
            return;

        m_currentIdx = idx;
        m_itemList[idx.row()].itemSignal.invoke(m_itemList[idx.row()].plugin ? m_itemList[idx.row()].plugin : this);
        m_listView->resetStatus(idx);
    });
    connect(m_listView, &DListView::activated, m_listView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row, const QString & name) {
        //不是本模块配置不响应
        if (!configContent(name))
            return ;
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

bool SoundWidget::configContent(const QString &configName)
{
    for (auto m : m_itemList) {
        if (configName == m.gsettingsName)
            return true;
    }
    return false;
}


int SoundWidget::showPath(const QString &path)
{
    auto getIndex = [=](const QString &name) {
        for (int i = 0; i < m_itemList.size(); ++i) {
            if (name == "Output" && m_itemList[ i ].itemText == tr("Output")) {
                return i;
            }

            if (name == "Input" && m_itemList[ i ].itemText == tr("Input")) {
                return i;
            }

            if (name == "Sound Effects" && m_itemList[ i ].itemText == tr("Sound Effects")) {
                return i;
            }

            if (name == "Devices" && m_itemList[ i ].itemText == tr("Devices")) {
                return i;
            }
        }

        return 0;
    };

    int index = getIndex(path);
    m_itemList[ index ].itemSignal.invoke(this);
    m_currentIdx = m_listView->model()->index(index, 0);
    m_listView->setCurrentIndex(m_currentIdx);

    return 0;
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
