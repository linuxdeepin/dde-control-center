/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "indexview.h"
#include "indexmodel.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QDebug>
#include <QScroller>

namespace dcc {
namespace keyboard{

IndexView::IndexView(QWidget *parent)
    :DListView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollPerPixel);
    QScroller::grabGesture(this->viewport(), QScroller::LeftMouseButtonGesture);
}

void IndexView::onClick(const QString &ch)
{
    IndexModel * model = qobject_cast<IndexModel*>(this->model());
    MetaData md;
    md.setText(ch);

    int index = model->indexOf(md);
    if(index != -1) {
        scrollTo(model->index(index,0),QAbstractItemView::PositionAtTop);
    }
}

void IndexView::showEvent(QShowEvent *e)
{
    QVariant var = indexAt(QPoint(5,10)).data();
    MetaData md = var.value<MetaData>();
    if(md.pinyin().count() > 0)
        m_section = md.pinyin().at(0).toUpper();

    QListView::showEvent(e);
}

void IndexView::scrollContentsBy(int dx, int dy)
{
    QVariant var = indexAt(QPoint(5,10)).data();
    MetaData md = var.value<MetaData>();
    if(md.pinyin().count() > 0)
        m_section = md.pinyin().at(0).toUpper();

    QListView::scrollContentsBy(dx,dy);
}

}
}
