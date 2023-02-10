//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "indexview.h"
#include "indexmodel.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QDebug>
#include <QScroller>

using namespace DCC_NAMESPACE;
IndexView::IndexView(QWidget *parent)
    :DListView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollPerPixel);
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
