//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "summaryitem.h"

#include <QVBoxLayout>

SummaryItem::SummaryItem(QFrame *parent)
    : DCC_NAMESPACE::SettingsItem(parent)
    , m_title(new QLabel)
    , m_details(new QLabel)
{
    setFixedHeight(36 * 3);

    m_title->setObjectName("UpdateSummary");

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    m_title->setAlignment(Qt::AlignHCenter);
    m_title->setWordWrap(true);
    m_details->setAlignment(Qt::AlignHCenter);

    layout->addStretch();
    layout->addWidget(m_title);
    layout->addSpacing(4);
    layout->addWidget(m_details);
    layout->addStretch();

    setLayout(layout);
}

void SummaryItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SummaryItem::setDetails(const QString &details)
{
    m_details->setText(details);
}
