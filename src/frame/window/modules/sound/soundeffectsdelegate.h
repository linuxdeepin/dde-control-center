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

#ifndef SOUNDEFFECTSDELEGATE_H
#define SOUNDEFFECTSDELEGATE_H

#include "window/namespace.h"

#include <QStyledItemDelegate>
#include <QAbstractItemDelegate>

namespace DCC_NAMESPACE {

namespace sound {

class SoundEffectsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    SoundEffectsDelegate(QObject *parent = nullptr);

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
Q_SIGNALS:
    void playBtnClicked(const QModelIndex &index) const;
    void hoverItemChange(int index);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    bool isPointInBtn(const QPoint &pt);
};
}
}


#endif // SOUNDEFFECTSDELEGATE_H
