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
    m_layout(new QVBoxLayout)
{
    m_layout->setMargin(0);
    m_layout->setSpacing(1);
    setLayout(m_layout);
}

void SettingsGroup::appendItem(SettingsItem *item)
{
    m_layout->addWidget(item);
    item->installEventFilter(this);

    updateHeadTail();
    updateHeight();
}

void SettingsGroup::removeItem(SettingsItem *item)
{
    m_layout->removeWidget(item);
    item->removeEventFilter(this);

    updateHeadTail();
    updateHeight();
}

void SettingsGroup::setSpacing(const int spaceing)
{
    m_layout->setSpacing(spaceing);

    updateHeight();
}

bool SettingsGroup::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::Resize)
        updateHeight();

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
    setFixedHeight(m_layout->sizeHint().height());
}

} // namespace dcc end
