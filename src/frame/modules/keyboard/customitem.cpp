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

#include "customitem.h"

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QFont>
#include <QEvent>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::keyboard;

CustomItem::CustomItem(QWidget *parent)
    : SettingsItem(parent)
{
    setMouseTracking(true);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(20,0,10,0);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText(tr("Shortcuts"));
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    layout->addStretch();

    m_shortKey = new ShortcutKey;
    layout->addWidget(m_shortKey);
    m_shortKey->setTextList(QStringList());

    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();
    m_shortcutEdit->installEventFilter(this);
    layout->addWidget(m_shortcutEdit);

    setLayout(layout);
    setFixedHeight(36);

}

void CustomItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void CustomItem::setShortcut(const QString &shortcut)
{
    m_accels = shortcut;

    QString list = shortcut;
    list = list.replace("<", "");
    list = list.replace(">", "-");
    list = list.replace("_L","");
    list = list.replace("_R", "");
    list = list.replace("Control", "Ctrl");

    m_shortKey->setTextList(list.split("-"));
    m_shortcutEdit->hide();
    m_shortKey->show();
}

QString CustomItem::text() const
{
    return m_accels;
}

void CustomItem::mouseReleaseEvent(QMouseEvent *e)
{
    if(!m_shortcutEdit->isVisible() && m_shortKey->rect().contains(m_shortKey->mapFromParent(e->pos())))
    {
        m_shortKey->hide();
        m_shortcutEdit->clear();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
        m_shortcutEdit->setPlaceholderText(tr("Please enter a shortcut"));

        Q_EMIT requestUpdateKey();
    }
    else
    {
        m_shortKey->show();
        m_shortcutEdit->hide();
    }
}
