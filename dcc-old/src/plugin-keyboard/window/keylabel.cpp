//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "keylabel.h"

#include <QStyle>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QApplication>

const QMap<QString, QString> &DisplaykeyMap = { {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"}, {"percent", "%"},
    {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"}, {"parenleft", "("},
    {"parenright", ")"}, {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"},
    {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"less", "<"}, {"greater", ">"}, {"question", "?"},
    {"minus", "-"}, {"equal", "="}, {"brackertleft", "["}, {"breckertright", "]"}, {"backslash", "\\"},
    {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."}, {"slash", "/"}, {"Up", "↑"},
    {"Left", "←"}, {"Down", "↓"}, {"Right", "→"}, {"asciitilde", "~"}, {"grave", "`"}, {"Control", "Ctrl"},
    {"Super_L", "Super"}, {"Super_R", "Super"}
};

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
