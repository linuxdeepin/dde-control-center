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

#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

namespace dcc {

SettingsGroup::SettingsGroup(QFrame *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout),
    m_updateHeightTimer(new QTimer(this))
{
    m_layout->setMargin(0);
    m_layout->setSpacing(1);

    m_updateHeightTimer->setSingleShot(true);
    m_updateHeightTimer->setInterval(1);

    connect(m_updateHeightTimer, &QTimer::timeout, this, &SettingsGroup::updateHeight, Qt::QueuedConnection);

    setLayout(m_layout);
}

void SettingsGroup::insertItem(const int index, SettingsItem *item)
{
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

void SettingsGroup::setSpacing(const int spaceing)
{
    m_layout->setSpacing(spaceing);

    m_updateHeightTimer->start();
}

int SettingsGroup::itemCount() const
{
    return m_layout->count();
}

bool SettingsGroup::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::Resize)
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

} // namespace dcc end
