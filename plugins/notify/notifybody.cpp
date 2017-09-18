/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "notifybody.h"
#include <QPainter>
#include <QDebug>

NotifyBody::NotifyBody(QWidget *parent) : QWidget(parent)
{
    m_td = new QTextDocument;
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(272, 50);
}

void NotifyBody::setText(const QString &text)
{
    QTextOption bodyOption;
    QFont bodyfont;

    bodyOption.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    bodyOption.setWrapMode(QTextOption::WordWrap);
    bodyfont.setFamily("Noto Sans CJK SC");
    bodyfont.setPixelSize(12);

    m_td->setDefaultTextOption(bodyOption);
    m_td->setDefaultFont(bodyfont);
    m_td->setTextWidth(this->width());
    m_td->setDocumentMargin(0);

    const QFontMetrics fm(bodyfont);

    m_td->setHtml(holdTextInRect(fm, text, this->rect()));

    update();
}

void NotifyBody::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setBrush(QBrush(Qt::transparent));
    painter.setPen(Qt::black);

    m_td->drawContents(&painter);

    QWidget::paintEvent(event);
}

const QString NotifyBody::holdTextInRect(const QFontMetrics &fm, const QString &text, const QRect &rect) const
{
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap | Qt::TextWrapAnywhere;

    if (rect.contains(fm.boundingRect(rect, textFlag, text)))
        return text;

    QString str(text + "...");

    while (true)
    {
        if (str.size() < 4)
            break;

        QRect boundingRect = fm.boundingRect(rect, textFlag, str);
        if (rect.contains(boundingRect))
            break;

        str.remove(str.size() - 4, 1);
    }

    return str;
}
