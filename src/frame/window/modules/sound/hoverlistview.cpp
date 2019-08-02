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

#include "hoverlistview.h"
#include "soundeffectsmodel.h"

#include <QScroller>
#include <QEnterEvent>
#include <QMouseEvent>

#include <QDebug>

using namespace DCC_NAMESPACE::sound;

typedef SoundEffectsModel::Role ERole;

HoverListView::HoverListView(QWidget *parent)
    : QListView(parent)
{
    setAutoScroll(false);
    setMouseTracking(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void HoverListView::leaveEvent(QEvent *event)
{
    auto model = qobject_cast<SoundEffectsModel *>(this->model());
    if (model) {
        model->setHoverIndex(-1);
    }

    QListView::enterEvent(event);
}

bool HoverListView::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonRelease: {
        if (!model())
            break;

        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        if (me->button() != Qt::LeftButton)
            break;

        auto pos = me->pos();
        auto voption = viewOptions();
        auto idx = indexAt(pos);
        voption.rect = visualRect(idx);

        if (SoundEffectsModel::getHoverImgRect(voption, idx).contains(pos)) {
            return true;
        }

        break;
    }
    default:
        break;
    }

    return QListView::viewportEvent(event);
}
