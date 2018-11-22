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

#include "keyboardcontrol.h"
#include "keyitem.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>

namespace dcc {
namespace keyboard{
KeyboardControl::KeyboardControl(QFrame *parent)
    : SettingsItem(parent),
      m_modifiers(false),
      m_keycount(0)
{
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::StrongFocus);
    setFixedHeight(180);
}

KeyboardControl::~KeyboardControl()
{
    KeyItem::deleteItems();
}

void KeyboardControl::setConflictString(const QStringList &list)
{
    m_conflicts = list;
}

void KeyboardControl::setPress(const QString &key, bool press)
{
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        if((*it)->mainKey() ==  key && !m_conflicts.contains(key))
        {
            (*it)->setPress(press);
        }
    }
    update();
}

void KeyboardControl::setConflicts(const QString &key, bool press)
{
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        if((*it)->mainKey() ==  key)
        {
            (*it)->setPress(press);
        }
    }
    update();
}

void KeyboardControl::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QList<KeyItem*> list = KeyItem::keyboards();
    QList<KeyItem*>::Iterator it = list.begin();
    for(; it != list.end(); it++)
    {
        (*it)->paint(&painter, rect());
    }
}
}
}
