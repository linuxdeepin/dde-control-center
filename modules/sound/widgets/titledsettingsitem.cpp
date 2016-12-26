/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "titledsettingsitem.h"

#include <QVBoxLayout>

TitledSettingsItem::TitledSettingsItem(QFrame *parent) :
    SettingsItem(parent),
    m_title(new QLabel),
    m_widget(nullptr)
{
    QVBoxLayout * layout = new QVBoxLayout(this);

    layout->addWidget(m_title);

    setLayout(layout);
}

QString TitledSettingsItem::title() const
{
    return m_title->text();
}

void TitledSettingsItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

QWidget *TitledSettingsItem::widget()
{
    return m_widget;
}

void TitledSettingsItem::setWidget(QWidget *widget)
{
    QVBoxLayout *hLayout = qobject_cast<QVBoxLayout*>(layout());

    if (m_widget) hLayout->removeWidget(m_widget);
    m_widget = widget;
    hLayout->addWidget(widget);
}
