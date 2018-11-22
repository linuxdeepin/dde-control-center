/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "copyrightwidget.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>
#include <QTextEdit>
#include <QFile>
#include <dboxwidget.h>
#include <QLocale>
#include <QFutureWatcher>
#include <QtConcurrent>

#include "widgets/labels/tipslabel.h"
#include "widgets/utils.h"

DWIDGET_USE_NAMESPACE

namespace dcc{
namespace systeminfo{

QPair<QString, QString> loadLicenses()
{
    const QString title = getLicense(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title");
    const QString body = getLicense(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body");

    return QPair<QString, QString>(title, body);
}

CopyrightWidget::CopyrightWidget(QWidget *parent)
    :ContentWidget(parent)
{
    TranslucentFrame* widget = new TranslucentFrame;
    widget->setObjectName("copyrightWidget");
    QVBoxLayout *layout =new QVBoxLayout();

    m_title = new TipsLabel;
    m_body = new TipsLabel;
    m_body->setWordWrap(true);

    layout->addWidget(m_title);
    layout->setAlignment(m_title, Qt::AlignCenter);
    layout->addWidget(m_body);

    widget->setLayout(layout);
    setContent(widget);
    setTitle(tr("Edition License"));

    QFutureWatcher<QPair<QString, QString>> *w = new QFutureWatcher<QPair<QString, QString>>(this);
    w->setFuture(QtConcurrent::run(loadLicenses));

    connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [=] {
        const auto r = w->result();

        m_title->setText(r.first);
        m_body->setText(r.second);
    });
}

}
}
