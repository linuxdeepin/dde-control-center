//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "versionprotocolwidget.h"
#include "utils.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QFutureWatcher>
#include <QtConcurrent>

using namespace DCC_NAMESPACE;

QPair<QString, QString> loadLicenses()
{
    const QString title = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title");
    const QString body = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body");
    return QPair<QString, QString>(title, body);
}

VersionProtocolWidget::VersionProtocolWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_title(new QLabel(this))
    , m_body(new QLabel(this))
{
    m_body->setWordWrap(true);

    QFrame *widget = new QFrame(this);

    m_mainLayout->setContentsMargins(10,10,11,10);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_title, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_body);
    m_mainLayout->addStretch();

    widget->setLayout(m_mainLayout);
    setLayout(m_mainLayout);
    setContentsMargins(0, 8, 0, 8);

    QFutureWatcher<QPair<QString, QString>> *w = new QFutureWatcher<QPair<QString, QString>>(parent);
    w->setFuture(QtConcurrent::run(loadLicenses));

    connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [ = ] {
        const auto r = w->result();
        m_title->setText(r.first);
        m_body->setText(r.second);
        Q_EMIT loadTextFinished();
    });
}

