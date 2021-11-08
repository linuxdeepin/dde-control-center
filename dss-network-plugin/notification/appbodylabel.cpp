/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#include "appbodylabel.h"

#include <QTextDocument>
#include <QEvent>
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QTextLine>
#include <QPaintEngine>
#include <QStyle>

AppBodyLabel::AppBodyLabel(QWidget *parent)
    : DLabel(parent)
    , m_alignment(Qt::AlignVCenter)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void AppBodyLabel::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }

#ifdef QT_DEBUG
    setToolTip(text);
#endif

    m_text = text;
    int oldLineCount = m_lineCount;

    updateLineCount();

    if (oldLineCount != m_lineCount)
        updateGeometry();

    update();
}

// return line count
static int drawText(QPainter *painter, const QRectF &rect, int lineHeight, QTextLayout *layout, Qt::TextElideMode mode)
{
    int lineCount = 0;
    qreal height = 0;
    QString text = layout->text();
    QTextOption &text_option = *const_cast<QTextOption *>(&layout->textOption());

    if (painter) {
        text_option.setTextDirection(painter->layoutDirection());
    }

    layout->beginLayout();

    QTextLine line = layout->createLine();
    QPointF offset = rect.topLeft();

    while (line.isValid()) {
        ++lineCount;
        height += lineHeight;

        if (height + lineHeight > rect.height()) {
            QString end_str = text.mid(line.textStart());

            if (painter)
                end_str = painter->fontMetrics().elidedText(end_str, mode, qRound(rect.width() - 1));

            layout->endLayout();
            layout->setText(end_str);

            text_option.setWrapMode(QTextOption::NoWrap);
            layout->beginLayout();
            line = layout->createLine();
            line.setLineWidth(rect.width() - 1);
            text = end_str;
        } else {
            line.setLineWidth(rect.width());
        }

        line.setPosition(offset);

        if (painter) {
            line.draw(painter, QPointF(0, 0));
        }

        offset.setY(offset.y() + lineHeight);

        if (height + lineHeight > rect.height())
            break;

        line = layout->createLine();
    }

    layout->endLayout();

    return lineCount;
}

QSize AppBodyLabel::sizeHint() const
{
    return QSize(width(), fontMetrics().height() * m_lineCount);
}

QSize AppBodyLabel::minimumSizeHint() const
{
    QSize size(width(), fontMetrics().height());

    return size;
}

void AppBodyLabel::setAlignment(Qt::Alignment alignment)
{
    m_alignment = alignment;
}

bool AppBodyLabel::resizeHint(int minLineCount)
{
    if (m_lineCount > minLineCount) {
        m_lineCount--;
        updateGeometry();
    }
    return minLineCount >= m_lineCount;
}

const QString AppBodyLabel::holdTextInRect(const QFontMetrics &fm, const QString &text, const QRect &rect) const
{
    const int textFlag = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    if (rect.contains(fm.boundingRect(rect, textFlag, text)))
        return text;

    QString str(text + "...");

    while (true) {
        if (str.size() < 4)
            break;

        QRect boundingRect = fm.boundingRect(rect, textFlag, str);
        if (rect.contains(boundingRect))
            break;

        str.remove(str.size() - 4, 1);
    }

    return str;
}

void AppBodyLabel::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    int oldLineCount = m_lineCount;
    updateLineCount();

    if (oldLineCount != m_lineCount)
        updateGeometry();
}

void AppBodyLabel::paintEvent(QPaintEvent *event)
{
    if (m_text.isEmpty())
        return;

    Q_UNUSED(event)
    QPainter pa(this);
    pa.setOpacity(m_opacity);
    QTextLayout layout(m_text, font(), this);
    QTextOption option;

    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    option.setAlignment(m_alignment);

    layout.setTextOption(option);

    QRect rect(this->rect());
    int lineHeight = fontMetrics().height();
    int lineCount = m_lineCount;

    while (lineCount > 1 && lineCount * lineHeight > height()) {
        --lineCount;
    }

    rect.setHeight(lineCount * lineHeight);
    rect = QStyle::alignedRect(layoutDirection(), m_alignment, rect.size(), this->rect());

    drawText(&pa, rect, lineHeight, &layout, Qt::ElideRight);
}

void AppBodyLabel::updateLineCount()
{
    QTextLayout layout(m_text, font());
    QTextOption option;

    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    option.setAlignment(m_alignment);

    layout.setTextOption(option);

    m_lineCount = drawText(nullptr, QRectF(QPointF(0, 0), QSizeF(width(), INT_MAX)), fontMetrics().height(), &layout, Qt::ElideNone);
}
