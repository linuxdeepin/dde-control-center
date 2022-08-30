// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "indexframe.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

namespace dcc {
namespace keyboard{

IndexFrame::IndexFrame(QFrame *parent):TranslucentFrame(parent)
{
    setMouseTracking(true);
    setFrameShape(QFrame::NoFrame);
    m_letters.clear();
    QFontMetrics fm(font());

    int max = -1;
    char ch = 'A';
    for(int i = 0; i<26; i++)
    {
        int w = fm.width(QChar(ch));
        if(w > max)
            max = w;

        m_letters.append(QString(ch));
        ch++;
    }

    m_w = max + 6;
    setFixedWidth(m_w);
    setMinimumHeight(m_w*26);

    m_pressed = false;
    m_index = 0;
}

void IndexFrame::setLetters(const QStringList &lists)
{
    m_letters.clear();
    QFontMetrics fm(font());

    QStringList::const_iterator it = lists.begin();
    int max = -1;
    for(; it != lists.end(); ++it)
    {
        if((*it).count() > 0)
        {
            QChar ch = (*it)[0];
            int w = fm.width(ch.toUpper());
            if(w > max)
                max = w;

            m_letters.append(QString(ch));
        }
    }

    m_w = max + 6;
    setFixedWidth(m_w);
    setMinimumHeight(m_w*m_letters.count());
}

void IndexFrame::paintEvent(QPaintEvent *)
{
    if(m_letters.isEmpty())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen = painter.pen();
    pen.setColor(Qt::white);
    int h = height()/m_letters.count();
    for(int i = 0; i<m_letters.count(); i++)
    {
        QRect r(0,h*i,m_w,h);
        painter.drawText(r, Qt::AlignCenter, m_letters[i]);
    }
    painter.setBrush(Qt::white);
    painter.setOpacity(m_pressed ? 0.6 : 0.3);
    painter.drawRoundRect(m_rect);
}

void IndexFrame::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    int count = m_letters.count();
    if (count <= m_index) {
        qDebug() << "IndexFrame err index:" << m_index << ", count:" << count;
        return;
    }

    m_pressed = true;
    Q_EMIT click(m_letters[m_index]);

    update();
}

void IndexFrame::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    m_pressed = false;

    update();
}

void IndexFrame::mouseMoveEvent(QMouseEvent *e)
{
    if (m_letters.isEmpty()) {
        return;
    }

    int h = height()/m_letters.count();
    int index = e->pos().y()/h;

    if(index < m_letters.count())
    {
        m_rect = QRect(0,h*index, m_w, h);
        QPoint center = m_rect.center();

        m_rect = QRect(0,0,m_w, m_w+2);
        m_rect.moveCenter(center);
        m_index = index;
    }
    update();
}

void IndexFrame::resizeEvent(QResizeEvent *e)
{
    m_pressed = false;
    m_rect = QRect();
    QFrame::resizeEvent(e);
    update();
}

void IndexFrame::leaveEvent(QEvent *event)
{
    m_rect = QRect();
    QFrame::leaveEvent(event);
    update();
}

}
}
