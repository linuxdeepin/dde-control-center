/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xiechuan <xiechuan@uniontech.com>
   *
   * Maintainer: xiechaun <xiechaun@uniontech.com>
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

#include "wallpaper.h"
#include "widgets/settingsgroup.h"
#include "dstyle.h"
#include "groupview.h"
#include "wallpapersnippet.h"

#include <DListView>

#include <QStandardItemModel>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::personalization;

Wallpaper::Wallpaper(QWidget *parent)
    : QWidget(parent)
    , m_model(new QStandardItemModel())
    , m_group(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_snippet(new WallpaperSnippet())
{
    initUi();
}

void Wallpaper::initUi()
{
    GroupView *viewItem = new GroupView();
    viewItem->setFixedHeight(56);
    DListView *listView = viewItem->getListView();
    listView->setModel(m_model);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setBackgroundType(DStyledItemDelegate::BackgroundType::NoBackground);
    listView->setSelectionMode(QAbstractItemView::NoSelection);
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    DStandardItem *it = new DStandardItem(tr("Select Wallpaper"));
    it->setCheckable(false);

    DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
    QStyleOption opt;
    editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
    editaction->setClickAreaMargins(QMargins(8, 8, 8, 8));
    m_model->appendRow(it);
    listView->setResizeMode(QListView::Fixed);
    it->setActionList(Qt::Edge::RightEdge, {editaction});
    connect(editaction, &QAction::triggered, [ = ] {
        // TODO
    });

    QHBoxLayout *m_mainLayout = new QHBoxLayout(this);
    m_group->appendItem(viewItem);
    m_group->appendItem(m_snippet);
    m_mainLayout->addWidget(m_group);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
}

void Wallpaper::setLockPaperPath(QString path)
{
    m_snippet->setLockPaperPath(path);
}

void Wallpaper::setDesktopPaperPath(QString path)
{
    m_snippet->setDesktopPaperPath(path);
}
