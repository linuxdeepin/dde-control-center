/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "customedititem.h"
#include <QPainter>
#include <QFont>
#include <QApplication>
#include <QMouseEvent>

using namespace dcc;
using namespace dcc::keyboard;
using namespace dcc::widgets;

CustomEditItem::CustomEditItem(QWidget *parent) :
    SettingsItem(parent),
    m_title(new QLabel)
{
    QHBoxLayout* layout = new QHBoxLayout();
    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();

    layout->setContentsMargins(20,0,10,0);
    layout->setSpacing(2);

    layout->addWidget(m_title);
    layout->addWidget(m_shortcutEdit);
    setLayout(layout);
    setFixedHeight(36);

    m_inter = new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this);

    connect(m_inter, &KeybingdingInter::KeyEvent, this, &CustomEditItem::onKeyEvent);
}

void CustomEditItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void CustomEditItem::setAccels(const QString &accels)
{
    m_accels = accels;
}

void CustomEditItem::setId(const QString &id)
{
    m_id = id;
}

QString CustomEditItem::getAccles() const
{
    return m_accels;
}

void CustomEditItem::onKeyEvent(bool press, QString shortcut)
{
    if(!press)
    {
        if(!m_shortcutEdit->isVisible())
            return;

        if(shortcut.isEmpty() || shortcut.toLower() == "escape"){
            m_shortcutEdit->hide();
            return;
        }

        if(shortcut == "BackSpace" || shortcut == "delete"){
            m_shortcutEdit->hide();
            m_accels = tr("None");
        }else{
            m_shortcutEdit->hide();
            if(m_accels != shortcut)
            {
                m_accels = shortcut;
                emit requestShortcutList(m_accels); //check accels
            }
        }
    }
    else
    {
        if(shortcut.isEmpty())
            m_shortcutEdit->hide();
    }
    update();
}

void CustomEditItem::onSave()
{
    emit requestShortcutList(m_accels);
}

void CustomEditItem::paintEvent(QPaintEvent *e)
{
    SettingsItem::paintEvent(e);
    if(m_shortcutEdit->isVisible())
    {
       return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = qApp->font();
    QFontMetrics fm(font);
    QString accels = m_accels;
    accels = accels.replace("Control", "Ctrl");
    accels = accels.replace("<", "");
    accels = accels.replace(">", "-");
    accels = accels.replace("Above_Tab", "`");
    accels = accels.replace("Super_L", "Super");

    QString conflict(tr("Conflict"));
    QStringList splits = m_accels.split("-");
    int right = this->rect().right() - 10;
    for(int i = splits.count() - 1; i>=0; --i)
    {

        QString str = splits.at(i);
        int w;
        if (str == "null" && m_display) {
            w = fm.width(conflict);
        } else {
            w = fm.width(str);
        }
        int h = (height() - fm.height()-6)/2;
        QRect r(0,0,w+8,height());
        r.moveRight(right);
        right = right - w - 12;
        painter.setBrush(palette().color(QPalette::Window));
        painter.save();
        painter.setPen(Qt::NoPen);
        painter.drawRoundRect(r.adjusted(0,h,0,-h));
        painter.restore();
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(m_contain ? 1.0 : 0.85);
        if(str == "null" && m_display)
        {
            QPen pen = painter.pen();
            QColor col = pen.color();
            pen.setColor(Qt::red);
            painter.setPen(pen);
            painter.drawText(r,Qt::AlignCenter,tr("Conflict"));
            pen.setColor(col);
            painter.setPen(pen);
        }
        else
            painter.drawText(r,Qt::AlignCenter,str);

        painter.restore();
        if(i == 0)
        {
            m_rect = QRect(r.topLeft(),this->rect().bottomRight());
            m_rect = m_rect.adjusted(0,h,0,-h);
        }
    }
}

void CustomEditItem::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if(!m_shortcutEdit->isVisible() && m_rect.contains(e->pos()))
    {
        QRect r = QRect(0,m_rect.y(),width()/2,m_rect.height());
        r.moveRight(m_rect.right());
        m_shortcutEdit->setGeometry(r);
        m_shortcutEdit->clear();
        m_inter->GrabScreen();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
        m_shortcutEdit->setPlaceholderText(tr("Please enter a new shortcut"));
    }
    else
    {
        m_shortcutEdit->hide();
    }
    update();
}

void CustomEditItem::mouseMoveEvent(QMouseEvent *e)
{
    m_contain = m_rect.contains(e->pos());
    update();
}

void CustomEditItem::leaveEvent(QEvent *)
{
    m_contain = false;

    update();
}
