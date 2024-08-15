// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "versionprotocolwidget.h"
#include <QFutureWatcher>
#include <QLabel>
#include <QLocale>
#include <QVBoxLayout>
#include <QtConcurrent>

using namespace DCC_NAMESPACE;

VersionProtocolWidget::VersionProtocolWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_title(new QLabel(this))
    , m_body(new QLabel(this))
{
    m_body->setWordWrap(true);

    QFrame *widget = new QFrame(this);

    m_mainLayout->setContentsMargins(10, 10, 11, 10);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_title, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_body);
    m_mainLayout->addStretch();

    widget->setLayout(m_mainLayout);
    setLayout(m_mainLayout);
    setContentsMargins(0, 8, 0, 8);
}

void VersionProtocolWidget::setLicense(const QPair<QString, QString> &license)
{
    m_title->setText(license.first);
    m_body->setText(license.second);
    Q_EMIT loadTextFinished();
}
