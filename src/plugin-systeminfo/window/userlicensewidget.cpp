//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "userlicensewidget.h"
#include <QVBoxLayout>
#include <QLocale>
#include <QtConcurrent>

using namespace DCC_NAMESPACE;

UserLicenseWidget::UserLicenseWidget(QWidget *parent)
    : QWidget(parent)
{
    QFrame *widget = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_body = new QLabel(this);
    m_body->setWordWrap(true);

    layout->setContentsMargins(10,10,11,10);
    layout->addWidget(m_body);
    layout->addStretch();

    widget->setLayout(layout);
    setLayout(layout);
    setContentsMargins(0, 8, 0, 8);
}

void UserLicenseWidget::setUserLicense(const QString &text)
{
    m_body->setText(text);
    emit loadTextFinished();
}
