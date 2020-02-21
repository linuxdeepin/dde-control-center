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

#include "widgets/editablenextpagewidget.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>

namespace dcc {

namespace widgets {

EditableNextPageWidget::EditableNextPageWidget(QWidget *parent)
    : SettingsItem(parent),

      m_name(new QLabel),
      m_selectIcon(new QLabel),
      m_edit(new QLineEdit),
      m_editButton(new DIconButton(this)),
      m_nextButton(new DIconButton(this))
{
    QHBoxLayout *centralLayout = new QHBoxLayout;

    m_editButton->setObjectName("EditButton");
    m_nextButton->setObjectName("NextButton");

    m_edit->setVisible(false);

    centralLayout->addWidget(m_name);
    centralLayout->addWidget(m_edit);
    centralLayout->addSpacing(5);
    centralLayout->addWidget(m_editButton);
    centralLayout->addStretch();
    centralLayout->addWidget(m_selectIcon);
    centralLayout->addSpacing(5);
    centralLayout->addWidget(m_nextButton);
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(20, 8, 10, 8);

    connect(m_nextButton, &DIconButton::clicked, this, &EditableNextPageWidget::acceptNextPage);
    connect(m_editButton, &DIconButton::clicked, this, &EditableNextPageWidget::toEditMode, Qt::QueuedConnection);
    connect(m_edit, &QLineEdit::editingFinished, this, &EditableNextPageWidget::toEchoMode, Qt::QueuedConnection);

    setLayout(centralLayout);
}

void EditableNextPageWidget::setTitle(const QString &title)
{
    m_name->setText(title);
}

void EditableNextPageWidget::setIcon(const QPixmap &icon)
{
    m_selectIcon->setPixmap(icon);
}

void EditableNextPageWidget::mousePressEvent(QMouseEvent *e)
{
    SettingsItem::mousePressEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    // ensure is echo mode
    if (m_name->isVisible())
        Q_EMIT selected();
}

void EditableNextPageWidget::keyPressEvent(QKeyEvent *e)
{
    e->accept();

    SettingsItem::keyPressEvent(e);
}

void EditableNextPageWidget::toEditMode()
{
    m_edit->setText(m_name->text());
    m_edit->setVisible(true);
    m_name->setVisible(false);
    m_editButton->setVisible(false);

    m_edit->setFocus();
}

void EditableNextPageWidget::toEchoMode()
{
    m_edit->clearFocus();

    const QString oldText = m_name->text();

    // TODO: text update should be trigger from model
//    m_name->setText(m_edit->text());
    m_name->setVisible(true);
    m_editButton->setVisible(true);
    m_edit->setVisible(false);

    Q_EMIT textChanged(oldText, m_edit->text());
}

}

}

