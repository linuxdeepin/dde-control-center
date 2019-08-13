/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "delappdelegate.h"
#include "defapplistview.h"

#include <dimagebutton.h>

#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE::defapp;

DelAppDelegate::DelAppDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *DelAppDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    if (!index.data(DefAppListView::DefAppIsUserRole).toBool()) {
        return nullptr;
    }

    // view class will delete these object later
    DImageButton *editor = new DImageButton(":/images/notify_close_normal.svg",
                                            ":/images/notify_close_hover.svg",
                                            ":/images/notify_close_press.svg", parent);

    editor->setFixedSize(QPixmap(editor->getNormalPic()).size());

    connect(editor, &DImageButton::clicked, this, [=] {
        Q_EMIT removeBtnClicked(index);
    });

    return editor;
}

void DelAppDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    const QRect &mRect = option.rect;
    editor->setGeometry(mRect.x() + mRect.right() - editor->width() - 10,
                        mRect.y() + (mRect.height() - editor->height()) / 2,
                        editor->width(),
                        editor->height());
}
