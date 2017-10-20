/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

CustomItem::CustomItem(KeyboardWork *work, QWidget *parent)
    :SettingsItem(parent),
      m_work(work),
      m_contain(false),
      m_info(nullptr)
{
    setMouseTracking(true);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(20,0,10,0);
    layout->setSpacing(2);

    m_title = new QLabel();
    m_title->setText(tr("Shortcut"));
    m_title->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignLeft);

    layout->addStretch();

    m_shortKey = new ShortcutKey;
    layout->addWidget(m_shortKey);
    m_shortKey->setTextList(QStringList() << tr("None"));

    m_shortcutEdit = new QLineEdit(this);
    m_shortcutEdit->setReadOnly(true);
    m_shortcutEdit->hide();
    m_shortcutEdit->installEventFilter(this);
    layout->addWidget(m_shortcutEdit);

    setLayout(layout);
    setFixedHeight(36);

    connect(m_work, &KeyboardWork::KeyEvent, this, &CustomItem::onKeyEvent);
}

void CustomItem::setInfo(ShortcutInfo *info)
{
    m_info = info;
    m_accels = info->accels;

    m_accels = m_accels.replace("<", "");
    m_accels = m_accels.replace(">", "-");

    m_shortKey->setTextList(m_accels.split("-"));
    m_shortKey->show();
}

QString CustomItem::text() const
{
    return m_accels;
}

void CustomItem::onKeyEvent(bool press, const QString &keylist)
{
    if(!press)
    {
        if(!m_shortcutEdit->isVisible())
            return;

        if (keylist == "BackSpace" || keylist == "escape") {
            m_shortcutEdit->hide();
        } else {
            QString in = keylist;
            in.replace("<", "");
            in.replace(">", "-");
            in.replace("_L", "");
            m_accels = keylist;
            m_accels = m_accels.replace("<", "");
            m_accels = m_accels.replace(">", "-");

            m_shortKey->setTextList(m_accels.split("-"));
            m_shortcutEdit->hide();
            m_shortKey->show();

            if (!keylist.isEmpty()){
                emit shortcut(keylist);
            }
        }
    }
}

void CustomItem::mousePressEvent(QMouseEvent *e)
{
    if(!m_shortcutEdit->isVisible() && m_shortKey->rect().contains(m_shortKey->mapFromParent(e->pos())))
    {
        m_shortKey->hide();
        m_shortcutEdit->clear();
        m_work->grabScreen();
        m_shortcutEdit->setFocus();
        m_shortcutEdit->show();
        m_shortcutEdit->setPlaceholderText(tr("Please enter a shortcut"));
    }
    else
    {
        m_shortKey->show();
        m_shortcutEdit->hide();
    }
}

bool CustomItem::eventFilter(QObject *watched, QEvent *event)
{
    if (m_shortcutEdit == watched && event->type() == QEvent::FocusOut)
        m_shortcutEdit->hide();

    return false;
}
