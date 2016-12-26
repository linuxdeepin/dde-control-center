/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "settingsgroup.h"
#include "settingsitem.h"
#include "settingsheaderitem.h"

#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

namespace dcc {
namespace widgets {


SettingsGroup::SettingsGroup(QFrame *parent) :
    TranslucentFrame(parent),
    m_layout(new QVBoxLayout),
    m_headerItem(nullptr),
    m_updateHeightTimer(new QTimer(this))
{
    m_layout->setMargin(0);
    m_layout->setSpacing(1);

    m_updateHeightTimer->setSingleShot(true);
    m_updateHeightTimer->setInterval(1);

    connect(m_updateHeightTimer, &QTimer::timeout, this, &SettingsGroup::updateHeight, Qt::QueuedConnection);

    setLayout(m_layout);
}

SettingsGroup::SettingsGroup(const QString &title, QFrame *parent)
    : SettingsGroup(parent)
{
    setHeaderVisible(!title.isEmpty());
    setAccessibleName(title);

    m_headerItem->setTitle(title);
}

SettingsGroup::~SettingsGroup()
{
    if (m_headerItem)
        m_headerItem->deleteLater();
}

void SettingsGroup::setHeaderVisible(const bool visible)
{
    if (visible)
    {
        if (!m_headerItem)
            m_headerItem = new SettingsHeaderItem;
        insertItem(0, m_headerItem);
    } else {
        if (m_headerItem)
        {
            m_headerItem->deleteLater();
            m_headerItem = nullptr;
        }
    }
}

void SettingsGroup::insertItem(const int index, SettingsItem *item)
{
    updateItemPaddings(item);

    m_layout->insertWidget(index, item);
    item->installEventFilter(this);

    updateHeadTail();
    m_updateHeightTimer->start();
}

void SettingsGroup::appendItem(SettingsItem *item)
{
    insertItem(m_layout->count(), item);
}

void SettingsGroup::removeItem(SettingsItem *item)
{
    m_layout->removeWidget(item);
    item->removeEventFilter(this);

    updateHeadTail();
    m_updateHeightTimer->start();
}

void SettingsGroup::moveItem(SettingsItem *item, const int index)
{
    const int oldIndex = m_layout->indexOf(item);
    if (oldIndex == index)
        return;

    m_layout->removeWidget(item);
    m_layout->insertWidget(index, item);

    const int max = m_layout->count() - 1;
    if (index == 0 || index == max ||
        oldIndex == 0 || oldIndex == max)
        updateHeadTail();
}

void SettingsGroup::setSpacing(const int spaceing)
{
    m_layout->setSpacing(spaceing);

    m_updateHeightTimer->start();
}

int SettingsGroup::itemCount() const
{
    return m_layout->count();
}

void SettingsGroup::clear()
{
    const int index = m_headerItem ? 1 : 0;
    const int count = m_layout->count();

    for (int i(index); i != count; ++i)
    {
        QLayoutItem *item = m_layout->takeAt(index);
        QWidget *w = item->widget();
        w->removeEventFilter(this);
        w->setParent(nullptr);
        delete item;
    }

    updateHeadTail();
    m_updateHeightTimer->start();
}

void SettingsGroup::setMargin(int left, int top, int right, int bottom)
{
    m_itemMarginLeft = left;
    m_itemMarginTop = top;
    m_itemMarginRight = right;
    m_itemMarginBottom = bottom;
}

SettingsItem *SettingsGroup::getItem(int index)
{
    if(index < 0)
        return NULL;

    if(index < itemCount())
    {
        return qobject_cast<SettingsItem *>(m_layout->itemAt(index)->widget());
    }

    return NULL;
}

bool SettingsGroup::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::Resize || event->type() == QEvent::Show || event->type() == QEvent::Hide)
        m_updateHeightTimer->start();

    return false;
}

void SettingsGroup::updateHeadTail()
{
    const int count = m_layout->count();
    for (int i(0); i != count; ++i)
    {
        SettingsItem *item = qobject_cast<SettingsItem *>(m_layout->itemAt(i)->widget());
        Q_ASSERT(item);

        item->setIsHead(i == 0);
        item->setIsTail(i == count - 1);
    }
}

void SettingsGroup::updateHeight()
{
    Q_ASSERT(sender() == m_updateHeightTimer);

    setFixedHeight(m_layout->sizeHint().height());
}

void SettingsGroup::updateItemPaddings(SettingsItem *item)
{
    QLayout * layout = qobject_cast<QLayout*>(item->layout());
    if (layout) {
        layout->setContentsMargins(m_itemMarginLeft,
                                   m_itemMarginTop,
                                   m_itemMarginRight,
                                   m_itemMarginBottom);
    }
}

}
}
