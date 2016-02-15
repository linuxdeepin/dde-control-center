/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QEvent>
#include <QGuiApplication>

#include <libdui_global.h>
#include <dthememanager.h>

#include "shortcutedit.h"
#include "shortcutdbus.h"

DWIDGET_USE_NAMESPACE

ShortcutEdit::ShortcutEdit(ShortcutDbus *dbus, QWidget *parent) :
    QFrame(parent),
    m_layout(new QHBoxLayout),
    m_label(new QLabel),
    m_edit(new QLineEdit),
    m_dbus(dbus)
{
    m_label->setObjectName("Label");
    m_edit->setObjectName("Edit");

    D_THEME_INIT_WIDGET(ShortcutEdit);

    m_label->installEventFilter(this);
    m_edit->setReadOnly(true);
    m_edit->setPlaceholderText(tr("Please input new shortcut"));
    m_edit->setMinimumSize(170, 25);
    m_edit->hide();

    m_layout->setMargin(0);
    m_layout->addWidget(m_label, 0, Qt::AlignRight|Qt::AlignVCenter);
    m_layout->addWidget(m_edit, 0, Qt::AlignRight|Qt::AlignVCenter);

    /// 此处的的第三个this参数必写，不然this被delete后
    /// 当m_dbus发出KeyReleaseEvent信号后照样会调用此匿名函数
    /// 此时，函数中使用this指针将导致控制中心崩溃
    connect(m_dbus, &ShortcutDbus::KeyReleaseEvent, this, [this](QString key){
        if(!editing())
            return;

        if(key.isEmpty() || key.toLower() == "escape"){
            quitEditState();
            return;
        }

        if(key.toLower() == "backspace" || key.toLower() == "delete"){
            key = "Valid_None";
        }else{
            QString result = m_dbus->CheckShortcutConflict(key);

            if(result != "Valid"){
                key = result+"_"+key;
            }else{
                key = "Valid_"+key;
            }
        }

        quitEditState();
        emit shortcutKeyFinished(key);
    });

    connect(m_dbus, &ShortcutDbus::KeyPressEvent, this, [this](const QString key){
        if(key.isEmpty())
            quitEditState();
    });

    setLayout(m_layout);
}

QString ShortcutEdit::shortcut() const
{
    return m_label->text();
}

bool ShortcutEdit::editing() const
{
    return m_edit->isVisible();
}

void ShortcutEdit::setShortcut(QString shortcut)
{
    m_label->setText(shortcut);
}

void ShortcutEdit::setEnableEdit(bool arg)
{
    if(arg){
        m_label->installEventFilter(this);
    }else{
        m_edit->hide();
        m_label->show();
        m_label->removeEventFilter(this);
    }
}

void ShortcutEdit::quitEditState()
{
    m_edit->hide();
    m_label->show();
}

bool ShortcutEdit::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_label){
        if(e->type() == QEvent::MouseButtonPress){
            m_label->hide();
            m_edit->show();
            m_dbus->GrabScreen();
        }
    }

    return false;
}

