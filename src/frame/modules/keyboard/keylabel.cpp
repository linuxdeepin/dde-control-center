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

#include "keylabel.h"
#include "shortcutmodel.h"

#include <QApplication>
#include <QPainter>

KeyLabel::KeyLabel(QWidget *parent)
    : QLabel(parent)
    , m_isEnter(false)
{
    setAlignment(Qt::AlignCenter);
}

KeyLabel::KeyLabel(const QString &text, QWidget *parent)
    : KeyLabel(parent)
{
    QString t;

    if (text.isEmpty()) {
        t = tr("None");
    } else {
        const QString &value = DisplaykeyMap[text];
        t = value.isEmpty() ? text : value;
    }

    setText(t);

    QFont font = qApp->font();
    QFontMetrics fm(font);

    setFixedWidth(fm.width(t) + 8);
}

void KeyLabel::setEnter(const bool enter)
{
    m_isEnter = enter;

    update();
}

void KeyLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    painter.setOpacity(m_isEnter ? 1.0 : 0.85);

    painter.setBrush(QColor(255, 255, 255, 0.2*255));
    painter.setPen(Qt::NoPen);

    QFont font = qApp->font();
    QFontMetrics fm(font);

    int h = (height() - fm.height()) / 2 - 2;

    painter.drawRoundRect(rect().marginsRemoved(QMargins(0, h, 0, h)), 30, 30);
}
