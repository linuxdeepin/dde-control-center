/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     Tinalu Shao <shaotianlu@uniontech.com>
*
* Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
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
#include "useritemdelegate.h"

#include <QPainter>
#include <QIcon>
#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QAbstractItemView>
#include <QListWidget>

#include <DPalette>
#include <DPaletteHelper>
#include <DStyleOption>
#include <DStyle>
#include <DStyledItemDelegate>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

UserItemDelegate::UserItemDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
}


QWidget *UserItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    if (!index.isValid())
        return nullptr;

    UserItem *editor = new UserItem(parent);
    editor->setTitle(index.data(NameRole).toString());
    editor->setPic(index.data(IconRole).toString());
    editor->setSelected(index.data(SelectedRole).toBool());
    editor->setOnline(index.data(OnlineStatusRole).toBool());
    return editor;

}

void UserItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    UserItem *edit = static_cast<UserItem *>(editor);
    edit->setTitle(index.data(NameRole).toString());
    edit->setPic(index.data(IconRole).toString());
    edit->setSelected(index.data(SelectedRole).toBool());
    edit->setOnline(index.data(OnlineStatusRole).toBool());
}

void UserItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void UserItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(index)

    return;
}

QSize UserItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return index.data(Qt::SizeHintRole).toSize();
}
