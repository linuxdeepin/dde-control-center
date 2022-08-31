// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "userlicensewidget.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/utils.h"
#include "window/utils.h"
#include "../../protocolfile.h"

#include <QVBoxLayout>
#include <QLocale>
#include <QtConcurrent>

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::systeminfo;

UserLicenseWidget::UserLicenseWidget(QWidget *parent)
    : ContentWidget(parent)
{
    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout();

    m_body = new TipsLabel;
    m_body->setWordWrap(true);

    layout->setContentsMargins(10,10,11,10);
    layout->addWidget(m_body);
    layout->addStretch();

    widget->setLayout(layout);
    setContent(widget);
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
