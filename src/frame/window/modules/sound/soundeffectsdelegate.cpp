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

#include "soundeffectsdelegate.h"
#include "soundeffectsmodel.h"

#include <dimagebutton.h>

#include <QPainter>
#include <QDebug>
#include <QPushButton>
#include <QMouseEvent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::sound;

typedef SoundEffectsModel::Role ERole;

SoundEffectsDelegate::SoundEffectsDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void SoundEffectsDelegate::paint(QPainter *painter,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    auto rect = SoundEffectsModel::getHoverImgRect(option, index);
    if (index.data(int(ERole::Playing)).toBool()) {
        QPixmap pm(index.data(int(ERole::AnimImg)).toString());
        painter->drawPixmap(rect, pm);
    } else if (index.data(int(ERole::HoverIndex)).toInt() == index.row()) {
        QPixmap pm(index.data(int(ERole::HoverBtnImg)).toString());
        painter->drawPixmap(rect, pm);
    }
}

bool SoundEffectsDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                       const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type()) {
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonPress: {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (me->button() != Qt::LeftButton)
            break;
        auto pos = me->pos();

        if (SoundEffectsModel::getHoverImgRect(option, index).contains(pos)) {
            if (event->type() == QEvent::MouseButtonPress) {
                Q_EMIT SoundEffectsDelegate::playBtnClicked(index);
            }
            return true;
        }
        break;
    }
    case QEvent::Type::MouseMove:
        Q_EMIT SoundEffectsDelegate::hoverItemChange(index.row());
        break;
    default:
        break;
    }

    return  QStyledItemDelegate::editorEvent(event, model, option, index);
}
