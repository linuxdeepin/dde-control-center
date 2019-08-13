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
#include "window/utils.h"
#include "modules/sound/soundworker.h"

#include <DStyleOption>

#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QMargins>

using namespace DCC_NAMESPACE::sound;

SoundWidget::SoundWidget(QWidget *parent)
    : QWidget(parent)
    , m_menuList(new QListView)
{
    setObjectName("Display");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setMaximumWidth(500);
    setMinimumWidth(300);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->addSpacing(10);

    m_menuList->setFrameShape(QFrame::NoFrame);
    layout->addWidget(m_menuList);
    initMenuUI();

    layout->addStretch(1);
    setLayout(layout);
}

void SoundWidget::initMenuUI()
{
    m_menuMethod = {
        { tr("Speaker"), QMetaMethod::fromSignal(&SoundWidget::requsetSpeakerPage)},
        { tr("Microphone"), QMetaMethod::fromSignal(&SoundWidget::requestMicrophonePage)},
        { tr("Advanced"), QMetaMethod::fromSignal(&SoundWidget::requestAdvancedPage)},
        { tr("Sound Effects"), QMetaMethod::fromSignal(&SoundWidget::requsetSoundEffectsPage)}
    };

    QStandardItemModel *listModel = new QStandardItemModel(this);
    for (auto mm : m_menuMethod) {
        QStandardItem *item = new QStandardItem(mm.menuText);
        item->setData(Dtk::RoundedBackground, Dtk::BackgroundTypeRole);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        listModel->appendRow(item);
    }
    m_menuList->setMinimumWidth(230);
    m_menuList->setModel(listModel);
    m_menuList->setCurrentIndex(listModel->index(0, 0));
    m_currentIdx = m_menuList->currentIndex();
    connect(m_menuList, &QListView::clicked, [ = ](const QModelIndex & idx) {
        if (idx == m_currentIdx)
            return;
        m_currentIdx = idx;
        m_menuMethod[idx.row()].method.invoke(this);
    });
}
