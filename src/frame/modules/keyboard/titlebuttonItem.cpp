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

#include "titlebuttonItem.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSpacerItem>
#include <QTimer>

TitleButtonItem::TitleButtonItem(QFrame *parent)
    :SettingsItem(parent)
{
    QHBoxLayout* layout =new QHBoxLayout();
    m_title = new QLabel();
    m_button = new QPushButton();

    m_title->setWordWrap(true);

    layout->addWidget(m_title);
    layout->addStretch();
    layout->addWidget(m_button);

    setLayout(layout);

    connect(m_button, SIGNAL(clicked()), this, SIGNAL(click()));
}

void TitleButtonItem::setTitle(const QString &title)
{
    m_title->setText(title);

    QTimer::singleShot(1, this, &TitleButtonItem::updateTitleSize);
}

void TitleButtonItem::setValue(const QString &value)
{
    m_button->setText(value);
}

void TitleButtonItem::updateTitleSize()
{
    int v = width() - m_button->width() - 32;
    if (m_title->fontMetrics().width(m_title->text()) > v)
        m_title->setFixedWidth(v);
}
