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

#include <QStyle>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>

KeyLabel::KeyLabel(const QString &text, QWidget *parent)
    : QWidget(parent)
    , m_isEnter(false)
    , m_text("")
{
    QString t;

    if (text.isEmpty()) {
        t = tr("None");
    } else {
        const QString &value = DisplaykeyMap[text];
        t = value.isEmpty() ? text : value;
    }

    m_text = t;

    QFont font = qApp->font();
    QFontMetrics fm(font);

    setMinimumWidth(fm.horizontalAdvance(t) + 18);
}

void KeyLabel::setEnter(const bool enter)
{
    m_isEnter = enter;

    update();
}

void KeyLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStylePainter stylePainter(this);
    QStyleOptionButton option;
    option.rect = rect();
    option.text = m_text;
    option.palette.setBrush(QPalette::Light, option.palette.base());
    option.palette.setBrush(QPalette::Dark, option.palette.base());
    option.palette.setBrush(QPalette::ButtonText, option.palette.highlight());
    option.palette.setBrush(QPalette::Shadow, Qt::transparent);
    stylePainter.drawControl(QStyle::CE_PushButton, option);
}
