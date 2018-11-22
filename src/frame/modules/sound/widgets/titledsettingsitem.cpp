/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
