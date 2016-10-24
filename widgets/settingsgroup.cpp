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

namespace dcc {

SettingsGroup::SettingsGroup(QFrame *parent) :
    QFrame(parent),
    m_layout(new QVBoxLayout)
{
    m_layout->setMargin(0);
    m_layout->setSpacing(2);
    setLayout(m_layout);
}

void SettingsGroup::appendItem(SettingsItem *item)
{
    m_layout->addWidget(item);
    item->installEventFilter(this);

    updateHeadTail();
}

void SettingsGroup::setSpacing(const int spaceing)
{
    m_layout->setSpacing(spaceing);

    updateHeight();
}

bool SettingsGroup::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::Resize) {

        updateHeight();
    }

    return false;
}

void SettingsGroup::updateHeadTail()
{
    SettingsItem * head = nullptr;
    SettingsItem * tail = nullptr;

    for (QObject * child : children()) {
        SettingsItem * item = qobject_cast<SettingsItem*>(child);
        if (item) {
            item->setIsHead(false);
            item->setIsTail(false);

            if (!head) head = item;
            tail = item;
        }
    }

    if (head) head->setIsHead(true);
    if (tail) tail->setIsTail(true);
}

void SettingsGroup::updateHeight()
{
    const int spaceing = m_layout->spacing();
    int height = 0;

    for (QObject * child : children()) {
        SettingsItem * item = qobject_cast<SettingsItem*>(child);
        if (item) {
            height += (item->height() + spaceing);
        }
    }

    setFixedHeight(height ? height - spaceing : 0);
}

} // namespace dcc end
