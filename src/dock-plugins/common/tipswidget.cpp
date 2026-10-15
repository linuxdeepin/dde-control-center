// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "tipswidget.h"

#include <QPainter>
#include <QAccessible>
#include <QTextDocument>

#define PADDING 4

namespace Dock{
TipsWidget::TipsWidget(QWidget *parent)
    : QFrame(parent)
    , m_type(SingleLine)
{
}

void TipsWidget::setText(const QString &text)
{
    bool typeChanged = m_type != TipsWidget::SingleLine;
    m_type = TipsWidget::SingleLine;
    // 如果传递的是富文本，获取富文本中的纯文本内容进行显示
    QTextDocument document;
    document.setHtml(text);
    // 同时去掉两边的空白信息，例如qBittorrent的提示
    QString newText = document.toPlainText().simplified();
    if (m_text == newText) {
        if (typeChanged) {
            updateGeometry();
            update();
        }
        return;
    }

    m_text = newText;

#if 0 //测试时可以使用下面的语句
    // FIXME:藏语字体绘制会有异常，设置高度时需要使用fontMetrics().boundingRect()去获取整体的边界矩形的高度，
    // 使用fontMetrics().height()去获取时，针对藏语这种字体，其高度和实际显示区域并不等同
    m_text = "བོད་སྐད་ཡིག་གཟུགས་ཚད་ལེན་ཚོད་ལྟའི་སྐོར་གྱི་རྗོད་ཚིག";
#endif

    updateGeometry();
    update();

#ifndef QT_NO_ACCESSIBILITY
    if (accessibleName().isEmpty()) {
        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
#endif
}

void TipsWidget::setTextList(const QStringList &textList)
{
    bool typeChanged = m_type != TipsWidget::MultiLine;
    m_type = TipsWidget::MultiLine;
    if (textList == m_textList) {
        if (typeChanged) {
            updateGeometry();
            update();
        }
        return;
    }
    m_textList = textList;

    updateGeometry();
    update();
}

/**
 * @brief TipsWidget::paintEvent 任务栏插件提示信息绘制
 * @param event
 */
void TipsWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QPen(palette().brightText(), 1));

    QTextOption option;
    option.setAlignment(Qt::AlignCenter);
    option.setWrapMode(QTextOption::NoWrap);

    switch (m_type) {
    case SingleLine: {
        painter.drawText(rect(), m_text, option);
    }
        break;
    case MultiLine: {
        int x = rect().x();
        int y = rect().y();
        if (m_textList.size() != 1) {
            option.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }
        for (const QString& text : m_textList) {
            int lineHeight = fontMetrics().boundingRect(text).height();
            painter.drawText(QRect(x, y, rect().width(), lineHeight), text, option);
            y += lineHeight;
        }
    } break;
    }
}

QSize TipsWidget::sizeHint() const
{
    const QFontMetrics fm = fontMetrics();

    switch (m_type) {
    case SingleLine: {
        return QSize(fm.horizontalAdvance(m_text),
                    fm.boundingRect(m_text).height());
    }
    case MultiLine: {
        int maxWidth = 0;
        int totalHeight = 0;
        for (const QString& text : m_textList) {
            maxWidth = qMax(maxWidth, fm.horizontalAdvance(text));
            totalHeight += fm.boundingRect(text).height();
        }

        return QSize(maxWidth, totalHeight);
    }
    }

    Q_UNREACHABLE_RETURN(QSize(0, 0));
}

bool TipsWidget::event(QEvent *event)
{
    if (event->type() == QEvent::FontChange) {
        updateGeometry();
        update();
    } else if (event->type() == QEvent::MouseButtonRelease
               && static_cast<QMouseEvent *>(event)->button() == Qt::RightButton) {
        return true;
    }
    return QFrame::event(event);
}

QSize TipsWidget::minimumSizeHint() const
{
    return sizeHint();
}

}
