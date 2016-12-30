/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "TitleEdit.h"
#include <QHBoxLayout>

using namespace dcc;
using namespace dcc::bluetooth;

TitleEdit::TitleEdit(QWidget *parent)
    : QWidget(parent),
      m_name(new QLabel),
      m_lineEdit(new QLineEdit),
      m_editWidget(new DImageButton)
{
    QHBoxLayout *mainlayout = new QHBoxLayout;
    m_lineEdit->setVisible(false);

    mainlayout->addWidget(m_name);
    mainlayout->addWidget(m_lineEdit);
    mainlayout->addSpacing(5);
    mainlayout->addWidget(m_editWidget);
    mainlayout->addStretch();
    mainlayout->setMargin(0);
    mainlayout->setSpacing(0);
    setLayout(mainlayout);

    connect(m_lineEdit, &QLineEdit::editingFinished, this, &TitleEdit::setName);
    connect(m_editWidget, &DImageButton::clicked, this, &TitleEdit::setEdit);
}

void TitleEdit::setName()
{
    if(m_name->text() != m_lineEdit->text()) {
        m_name->setText(m_lineEdit->text());
        m_lineEdit->setVisible(false);
        m_name->setVisible(true);
        emit requestSetBluetoothName(m_lineEdit->text());
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
