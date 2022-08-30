// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "keylabel.h"
#include "shortcutmodel.h"

#include <QStyle>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>

#ifdef WINDOW_MODE
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

    setMinimumWidth(fm.width(t) + 18);
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
#else
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

    painter.setBrush(QColor(255, 255, 255, 0.2 * 255));
    painter.setPen(Qt::NoPen);

    QFont font = qApp->font();
    QFontMetrics fm(font);

    int h = (height() - fm.height()) / 2 - 2;

    painter.drawRoundRect(rect().marginsRemoved(QMargins(0, h, 0, h)), 30, 30);
}
#endif
