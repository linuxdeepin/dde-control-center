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

#include "lineeditwidget.h"

#include <QVBoxLayout>

namespace dcc {
namespace widgets {

LineEditWidget::LineEditWidget(QFrame *parent)
    : SettingsItem(parent),

      m_title(new QLabel),
      m_edit(new QLineEdit)
{
    m_title->setFixedWidth(140);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_title);
    m_mainLayout->addWidget(m_edit);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    setLayout(m_mainLayout);
    setObjectName("LineEditWidget");
    setFixedHeight(36);
}

void LineEditWidget::addRightWidget(QWidget *widget)
{
    m_mainLayout->addWidget(widget);
}

void LineEditWidget::setReadOnly(const bool state)
{
    m_edit->setReadOnly(state);
}

void LineEditWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
    m_edit->setAccessibleName(title);
}

void LineEditWidget::setText(const QString &text)
{
    m_edit->setText(text);
}

void LineEditWidget::setPlaceholderText(const QString &text)
{
    m_edit->setPlaceholderText(text);
}

void LineEditWidget::mousePressEvent(QMouseEvent *e)
{
    SettingsItem::mousePressEvent(e);

    m_edit->setFocus();
}

}
}
