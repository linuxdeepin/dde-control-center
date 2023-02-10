//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "titlebuttonItem.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSpacerItem>
#include <QTimer>
using namespace DCC_NAMESPACE;
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
    if (m_title->fontMetrics().horizontalAdvance(m_title->text()) > v)
        m_title->setFixedWidth(v);
}
