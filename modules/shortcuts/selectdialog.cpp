/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QPushButton>

#include <libdui_global.h>
#include <dthememanager.h>
#include <dtextbutton.h>

#include "selectdialog.h"

DWIDGET_USE_NAMESPACE

SelectDialog::SelectDialog(QWidget *parent):
    QFrame(parent),
    m_label(new QLabel),
    m_animation(new QPropertyAnimation(this, "maximumHeight", this))
{
    D_THEME_INIT_WIDGET(SelectDialog);

    m_widget = this;

    m_label->setWordWrap(true);

    QLabel *icon = new QLabel;
    icon->setPixmap(QPixmap(":/images/ico_warning.png"));
    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(m_label, 0, Qt::AlignLeft|Qt::AlignVCenter);
    hlayout1->addWidget(icon, 0, Qt::AlignRight|Qt::AlignVCenter);

    DTextButton *accept_button = new DTextButton(tr("Replace"));
    DTextButton *cancel_button = new DTextButton(tr("Cancel"));
    QHBoxLayout *hlayout2 = new QHBoxLayout;
    hlayout2->addStretch(1);
    hlayout2->addWidget(accept_button, 0, Qt::AlignBottom);
    hlayout2->addWidget(cancel_button, 0, Qt::AlignBottom);

    connect(accept_button, &QPushButton::clicked, this, &SelectDialog::replace);
    connect(cancel_button, &QPushButton::clicked, this, &SelectDialog::cancel);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);

    setLayout(layout);

    connect(m_animation, &QPropertyAnimation::finished, [&]{
        if(height()==0)
            emit contracted();
    });
}

QStringList SelectDialog::keyWords() const
{
    return QStringList();
}

void SelectDialog::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        setText(datas.toString());
    }
}

QVariant SelectDialog::getData()
{
    return text();
}

QWidget *SelectDialog::widget() const
{
    return m_widget;
}

QString SelectDialog::text() const
{
    return m_label->text();
}

void SelectDialog::setText(const QString &text)
{
    m_label->setText(text);
}

void SelectDialog::expansion()
{
    setFixedHeight(0);
    m_animation->setStartValue(0);
    m_animation->setEndValue(sizeHint().height());
    m_animation->start();
    emit expanded();
}

void SelectDialog::contraction()
{
    m_animation->setStartValue(sizeHint().height());
    m_animation->setEndValue(0);
    m_animation->start();
}

