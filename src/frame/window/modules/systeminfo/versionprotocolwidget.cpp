// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "versionprotocolwidget.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/utils.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QFutureWatcher>
#include <QtConcurrent>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::systeminfo;

QPair<QString, QString> loadLicenses()
{
    const QString title = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title");
    const QString body = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body");
    return QPair<QString, QString>(title, body);
}

VersionProtocolWidget::VersionProtocolWidget(QWidget *parent)
    : ContentWidget(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_title(new TipsLabel)
    , m_body(new TipsLabel)
{
    m_body->setWordWrap(true);

    TranslucentFrame *widget = new TranslucentFrame;

    m_mainLayout->setContentsMargins(10,10,11,10);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_title, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_body);
    m_mainLayout->addStretch();

    widget->setLayout(m_mainLayout);
    setContent(widget);
    setContentsMargins(0, 8, 0, 8);

    QFutureWatcher<QPair<QString, QString>> *w = new QFutureWatcher<QPair<QString, QString>>(this);
    w->setFuture(QtConcurrent::run(loadLicenses));

    connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [ = ] {
        const auto r = w->result();
        m_title->setText(r.first);
        m_body->setText(r.second);
        Q_EMIT loadTextFinished();
    });
}

