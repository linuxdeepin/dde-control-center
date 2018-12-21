/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "shortcutkey.h"
#include "keylabel.h"

#include <QMouseEvent>
#include <QDebug>

ShortcutKey::ShortcutKey(QWidget *parent) : QWidget(parent)
{

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(5);

    setLayout(m_mainLayout);
    setStyleSheet("background: transparent;");
}

void ShortcutKey::setTextList(const QStringList &list)
{
    for (KeyLabel *label : m_list) {
        m_mainLayout->removeWidget(label);
        label->deleteLater();
    }

    m_list.clear();

    for (const QString &key : list) {
        KeyLabel *label = new KeyLabel(key);
        m_list << label;
        m_mainLayout->addWidget(label);
    }

    adjustSize();
}

void ShortcutKey::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);

    for (KeyLabel *label : m_list)
        label->setEnter(true);
}

void ShortcutKey::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    for (KeyLabel *label : m_list)
        label->setEnter(false);
}
