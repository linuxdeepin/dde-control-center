/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *             Tianlu Shao <shaotianlu@uniontech.com>
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

#include "versionprotocolwidget.h"

#include "utils.h"

#include <QFutureWatcher>
#include <QLabel>
#include <QLocale>
#include <QVBoxLayout>
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

    m_mainLayout->setContentsMargins(10, 10, 11, 10);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addWidget(m_title, 0, Qt::AlignCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(m_body);
    m_mainLayout->addStretch();

    widget->setLayout(m_mainLayout);
    setLayout(m_mainLayout);
    setContentsMargins(0, 8, 0, 8);

    QFutureWatcher<QPair<QString, QString>> *w =
            new QFutureWatcher<QPair<QString, QString>>(parent);
    w->setFuture(QtConcurrent::run(loadLicenses));

    connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [=] {
        const auto r = w->result();
        m_title->setText(r.first);
        m_body->setText(r.second);
        Q_EMIT loadTextFinished();
    });
}
