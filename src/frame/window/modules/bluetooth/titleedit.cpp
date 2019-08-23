/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "titleedit.h"

#include <dimagebutton.h>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

TitleEdit::TitleEdit(QWidget *parent)
    : QWidget(parent)
    , m_name(new QLabel)
    , m_lineEdit(new QLineEdit)
{
    QHBoxLayout *mainlayout = new QHBoxLayout;
    m_lineEdit->setVisible(false);
    mainlayout->addWidget(m_name);
    mainlayout->addWidget(m_lineEdit);
    mainlayout->addSpacing(5);
    DImageButton *editWidget = new DImageButton;
    editWidget->setNormalPic(":/bluetooth/themes/light/icons/edit_normal.png");
    editWidget->setHoverPic(":/bluetooth/themes/light/icons/edit_hover.png");
    editWidget->setPressPic(":/bluetooth/themes/light/icons/edit_press.png");
    mainlayout->addWidget(editWidget);
    mainlayout->addStretch();
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    setLayout(mainlayout);

    connect(m_lineEdit, &QLineEdit::editingFinished, this, &TitleEdit::setName);
    connect(editWidget, &DImageButton::clicked, this, &TitleEdit::setEdit);
}

void TitleEdit::setName()
{
    if (m_name->text() != m_lineEdit->text()) {
        m_name->setText(m_lineEdit->text());
        m_lineEdit->setVisible(false);
        m_name->setVisible(true);
        Q_EMIT requestSetBluetoothName(m_lineEdit->text());
    }
}

void TitleEdit::setEdit()
{
    m_name->setVisible(false);
    m_lineEdit->setVisible(true);
}

void TitleEdit::setTitle(const QString &title)
{
    m_name->setText(title);
    m_lineEdit->setText(title);
}
