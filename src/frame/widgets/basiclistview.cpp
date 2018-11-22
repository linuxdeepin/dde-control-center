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

#include "widgets/basiclistview.h"

#include <QDebug>
#include <QTimer>

namespace dcc {

namespace widgets {

BasicListView::BasicListView(QWidget *parent)
    : QListView(parent),

      m_autoFitHeight(true)
{
//    setStyleSheet("background-color: red;");
    setFrameStyle(QFrame::NoFrame);
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollPerPixel);
    setContentsMargins(0, 0, 0, 0);
    setSpacing(0);
    setSelectionMode(NoSelection);
}

QSize BasicListView::sizeHint() const
{
    int height = 0;
    const int count = model()->rowCount();
    for (int i(0); i != count; ++i)
        height += sizeHintForRow(i);

    return QSize(QListView::sizeHint().width(), height);
}

void BasicListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);

    connect(model, &QAbstractItemModel::layoutChanged, this, &BasicListView::onContentHeightChanged, Qt::QueuedConnection);
    connect(model, &QAbstractItemModel::rowsInserted, this, &BasicListView::onContentHeightChanged, Qt::QueuedConnection);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &BasicListView::onContentHeightChanged, Qt::QueuedConnection);

    QTimer::singleShot(1, this, &BasicListView::onContentHeightChanged);
}

void BasicListView::onContentHeightChanged()
{
    if (!m_autoFitHeight)
        return;

    const int h = sizeHint().height();
    setMaximumHeight(h);
}

void BasicListView::leaveEvent(QEvent *e)
{
    QListView::leaveEvent(e);

    Q_EMIT entered(QModelIndex());
}

void BasicListView::setAutoFitHeight(const bool fit)
{
    m_autoFitHeight = fit;

    if (fit)
        onContentHeightChanged();
}

}

}
