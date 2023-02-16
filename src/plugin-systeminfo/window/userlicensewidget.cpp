//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "userlicensewidget.h"
#include "widgets/utils.h"
#include "protocolfile.h"

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

    QFutureWatcher<QString> *w = new QFutureWatcher<QString>(this);
    if (DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        w->setFuture(QtConcurrent::run(ProtocolFile::getEulerEnduserAgreement));
    } else {
        w->setFuture(QtConcurrent::run(ProtocolFile::getEnduserAgreement));
    }

    connect(w, &QFutureWatcher<QString>::finished, this, [ = ] {
        const QString r = w->result();
        m_body->setText(r);
        Q_EMIT loadTextFinished();
    });
}
