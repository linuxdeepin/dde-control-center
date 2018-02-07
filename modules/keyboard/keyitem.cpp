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

#include "keyitem.h"
#include <QPainter>
#include <QPixmap>

namespace dcc {
namespace keyboard{

QList<KeyItem*> KeyItem::m_keys;

KeyItem::KeyItem(int row, int col, const QString &main, const QString &vice)
    :m_mainKey(main),
      m_viceKey(vice),
      m_row(row),
      m_col(col),
      m_keycode(-1),
      m_press(false),
      m_conflict(false)
{

}

KeyItem::KeyItem(int row, int col, int key, const QString &main, const QString &vice)
    :m_mainKey(main),
      m_viceKey(vice),
      m_row(row),
      m_col(col),
      m_keycode(key),
      m_press(false),
      m_conflict(false)
{
    QStringList list;
    list<<"Ctrl"<<"Alt"<<"Super"<<"Shift"<<"Fn";
    m_modifies = list.contains(m_mainKey);
}

KeyItem::~KeyItem()
{

}

void KeyItem::setMainKey(const QString &key)
{
    m_mainKey = key;
}

QString KeyItem::mainKey() const
{
    return m_mainKey;
}

void KeyItem::setViceKey(const QString &key)
{
    m_viceKey = key;
}

QString KeyItem::viceKey() const
{
    return m_viceKey;
}

void KeyItem::setPress(bool press)
{
    m_press = press;
}

void KeyItem::setConflict(bool conflict)
{
    m_conflict = conflict;
}

QList<KeyItem *> KeyItem::keyboards()
{
    if(m_keys.count() == 0)
    {
        m_keys<<(new KeyItem(0,0,9,"Esc"))<<(new KeyItem(0, 1, 67, "F1"))
                <<(new KeyItem(0, 2, 68, "F2"))<<(new KeyItem(0, 3, 69,"F3"))
                <<(new KeyItem(0, 4, 70,"F4"))<<(new KeyItem(0, 5,71,"F5"))
                <<(new KeyItem(0, 6,72, "F6"))<<(new KeyItem(0, 7, 73, "F7"))
                <<(new KeyItem(0, 8,74,"F8"))<<(new KeyItem(0,9,75, "F9"))
                <<(new KeyItem(0,10,76, "F10"))<<(new KeyItem(0,11,95,"F11"))
                <<(new KeyItem(0,12,96, "F12"))<<(new KeyItem(0,13,"Power"))
                <<(new KeyItem(1,0,49, "~"))<<(new KeyItem(1,1, 10,"1"))
                <<(new KeyItem(1,2,11,"2"))<<(new KeyItem(1,3,12,"3"))
                <<(new KeyItem(1,4,13,"4"))<<(new KeyItem(1,5,14,"5"))
                <<(new KeyItem(1,6,15,"6"))<<(new KeyItem(1,7,16,"7"))
                <<(new KeyItem(1,8,17, "8"))<<(new KeyItem(1,9,18,"9"))
                <<(new KeyItem(1,10,19,"0"))<<(new KeyItem(1,11,20, "-"))
                <<(new KeyItem(1,12,21,"="))<<(new KeyItem(1,13,22,"Delete"))
                <<(new KeyItem(2,0,23,"Tab"))<<(new KeyItem(2,1,24,"Q"))
                <<(new KeyItem(2,2,25,"W"))<<(new KeyItem(2,3,26,"E"))
                <<(new KeyItem(2,4,27,"R"))<<(new KeyItem(2,5,28,"T"))
                <<(new KeyItem(2,6,29,"Y"))<<(new KeyItem(2,7,30,"U"))
                <<(new KeyItem(2,8,31,"I"))<<(new KeyItem(2,9,32,"O"))
                <<(new KeyItem(2,10,33,"P"))<<(new KeyItem(2,11,34,"["))
                <<(new KeyItem(2,12,35,"]"))<<(new KeyItem(2,13,51,"\\"))
                <<(new KeyItem(3,0,-1,"CAPS"))<<(new KeyItem(3,1,38,"A"))
                <<(new KeyItem(3,2,39,"S"))<<(new KeyItem(3,3,40,"D"))
                <<(new KeyItem(3,4,41,"F"))<<(new KeyItem(3,5,42,"G"))
                <<(new KeyItem(3,6,43,"H"))<<(new KeyItem(3,7,44,"J"))
                <<(new KeyItem(3,8,45,"K"))<<(new KeyItem(3,9,46,"L"))
                <<(new KeyItem(3,10,47,";"))<<(new KeyItem(3,11,48,"''"))
                <<(new KeyItem(3,12,36,"ENTER"))
                <<(new KeyItem(4,0,50,"Shift"))<<(new KeyItem(4,1,52,"Z"))
                <<(new KeyItem(4,2,53,"X"))<<(new KeyItem(4,3,54,"C"))
                <<(new KeyItem(4,4,55,"V"))<<(new KeyItem(4,5,56,"B"))
                <<(new KeyItem(4,6,57,"N"))<<(new KeyItem(4,7,58,"M"))
                <<(new KeyItem(4,8,59,",<"))<<(new KeyItem(4,9,60,">."))
                <<(new KeyItem(4,10,61,"?/"))<<(new KeyItem(4,11,62,"Shift"))
                <<(new KeyItem(5,0,-1,"Fn"))<<(new KeyItem(5,1,37,"Ctrl"))
                <<(new KeyItem(5,2, 64,"Alt"))<<(new KeyItem(5,3,133,"Super"))
                <<(new KeyItem(5,4,65,"Space"))<<(new KeyItem(5,6,108,"Alt"))
                <<(new KeyItem(5,7,113,"Left"))<<(new KeyItem(5,8, 111, "Up"))
                <<(new KeyItem(5,9,114,"Right"))<<(new KeyItem(5,10,116,"Down"));
    }

    return m_keys;
}

void KeyItem::deleteItems()
{
    qDeleteAll(m_keys);
    m_keys.clear();
}

void KeyItem::paint(QPainter *painter, const QRect &rect)
{
    QFont font = painter->font();
    QPen pen = painter->pen();
    painter->setRenderHint(QPainter::Antialiasing);
    pen.setBrush(Qt::white);
    painter->setPen(pen);
    font.setPixelSize(10);
    int height = (rect.height() - 10)/6;
    int count = (m_row == 0) ? 14 :(16- (m_row == 1 ? 2 : m_row));
    int width;

    if(m_row == 0)
    {
        if(m_mainKey == "Power") {
            width = rect.width()/count;
            m_rect = QRect(m_col*width+width/3,m_row*height+height/3, 11, 11);
        } else {
            width = rect.width()/count;
            m_rect = QRect(m_col*width,m_row*height, width, height);}
    }
    else if(m_row == 1)
    {
        width = (rect.width()-20)/count;
        if(m_mainKey == "Delete")
        {
            m_rect = QRect(m_col*width,m_row*height, width + 20, height);
        }
        else
        {
            m_rect = QRect(m_col*width,m_row*height, width, height);
        }
    }
    else if(m_row == 2)
    {
        width = (rect.width()/count);
        m_rect = QRect(m_col*width,m_row*height, width, height);
    }
    else if(m_row == 3)
    {
        int space = 40;
        width = ((rect.width() - space) /count);
        if(m_mainKey == "CAPS")
        {
            m_rect = QRect(m_col*width,m_row*height, rect.width()/count, height);
        }
        else if(m_mainKey == "ENTER")
        {
            m_rect = QRect(m_col*width + space/2,m_row*height, width + space/2, height);
        }
        else
        {
            m_rect = QRect(m_col*width + space/2,m_row*height, width, height);
        }
    }
    else if(m_row == 4)
    {
        int space = 40;
        width = ((rect.width() - space) /count);
        if(m_mainKey == "Shift")
        {
            if(m_col == 0)
            {
                m_rect = QRect(m_col*width,m_row*height, rect.width()/count, height);
            }
            else
            {
                m_rect = QRect(m_col*width+space/2,m_row*height, width + space/2, height);
            }
        }
        else
        {
            m_rect = QRect(m_col*width + space/2,m_row*height, width, height);
        }
    }
    else if(m_row == 5)
    {
        int expand = 120;
        width = (rect.width() - expand*0.8)/11;
        m_rect = QRect(m_col*width,m_row*height, width, height + 10);
        if(m_mainKey == "Space")
        {
            m_rect = QRect(m_col*width, m_row*height, width + expand, height + 10);
        } else if (m_mainKey == "Super") {
            m_rect = QRect(m_col*width+width/3, m_row*height+height/2, 11 , 11);
        }
        else if(m_mainKey == "Alt" && m_col == 6)
        {
            m_rect = QRect(m_col*width + expand, m_row*height, width, height + 10);
        }
        else if(m_col > 6)
        {
            QRect r = QRect(7*width + expand, m_row*height, rect.width() - 7*width - expand, height + 10);
            int del = 7;
            if(m_mainKey == "Left")
            {
                QRect left = QRect(r.x(), r.y() + r.height()/2, r.width()/3, r.height()/2);
                m_rect = left;
                QPolygon gon;
                painter->save();
                painter->setBrush(Qt::white);
                gon<<(left.center() + QPoint(del,0))<<(left.topRight() + QPoint(0,del))
                  <<(left.bottomRight() - QPoint(0,del))<<(left.center() + QPoint(del,0));
                painter->translate(-left.width()/2, 0);
                painter->drawPolygon(gon);
                painter->restore();
            }
            else if(m_mainKey == "Up")
            {
                QRect up = QRect(r.x() + r.width()/3, r.y(), r.width()/3, r.height()/2);
                m_rect = up;
                QPolygon gon;
                painter->save();
                painter->setBrush(Qt::white);
                gon<<(up.center() + QPoint(0,del))<<(up.bottomLeft() + QPoint(del,0))
                  <<(up.bottomRight() - QPoint(del,0))<<(up.center() + QPoint(0,del));
                painter->translate(0,-up.height()/2);
                painter->drawPolygon(gon);
                painter->restore();
            }
            else if(m_mainKey == "Right")
            {
                QRect right = QRect(r.x() + r.width()*2/3, r.y() + r.height()/2, r.width()/3, r.height()/2);
                m_rect = right;
                QPolygon gon;
                painter->save();
                painter->setBrush(Qt::white);
                gon<<(right.center() - QPoint(del,0))<<(right.topLeft() + QPoint(0,del))
                  <<(right.bottomLeft() - QPoint(0,del))<<(right.center() - QPoint(del,0));
                painter->translate(right.width()/2, 0);
                painter->drawPolygon(gon);
                painter->restore();
            }
            else
            {
                QRect down = QRect(r.x() + r.width()/3, r.y() + r.height()/2, r.width()/3, r.height()/2);
                m_rect = down;
                QPolygon gon;
                down.moveTop(down.center().y());
                painter->save();
                painter->setBrush(Qt::white);
                gon<<(down.center() - QPoint(0,del))<<(down.topLeft() + QPoint(del,0))
                  <<(down.topRight() - QPoint(del,0))<<(down.center() - QPoint(0,del));
                painter->drawPolygon(gon);
                painter->restore();
            }
            if(m_press)
            {
                painter->save();
                if(m_modifies || m_conflict)
                    painter->setBrush(Qt::red);
                else
                    painter->setBrush(Qt::green);
                painter->setOpacity(0.5);
                painter->drawRect(m_rect);
                painter->restore();
            }
            return;
        }
//        painter->fillRect(m_rect, QColor(qrand()%255,qrand()%255,qrand()%255));
    }
    painter->setFont(font);
//    painter->drawText(m_rect,Qt::AlignCenter, m_mainKey);
    if(m_mainKey == "Power") {
        painter->drawPixmap(m_rect, QPixmap(":/keyboard/themes/common/icons/power.png"));
    } else if (m_mainKey == "Super") {
        painter->drawPixmap(m_rect, QPixmap(":/keyboard/themes/common/icons/super_key.png"));
    }else {
            painter->drawText(m_rect,Qt::AlignCenter, m_mainKey);
    }
    if(m_press && (m_keycode != 108 && m_keycode != 62))
    {
        painter->save();
        if(m_modifies || m_conflict)
            painter->setBrush(Qt::red);
        else
            painter->setBrush(Qt::green);
        painter->setOpacity(0.5);
        painter->drawRect(m_rect);
        painter->restore();
    }
}
}
}
