/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "userlicensewidget.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/tipslabel.h"
#include "widgets/utils.h"
#include "window/utils.h"

#include <QVBoxLayout>
#include <QLocale>
#include <QFutureWatcher>
#include <QtConcurrent>

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::systeminfo;

static QString loadLicenses()
{
    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer) {

        const QString serverPath = getLicensePath("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/End-User-License-Agreement-Server-CN-%1.txt", "");
        if ( QFile::exists(serverPath)) {
            const QString serverbody = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/End-User-License-Agreement-Server-CN-%1.txt", "");
            return serverbody;
        } else {
            const QString oldPody = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt", "");
            return oldPody;
        }
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosHome) {
        const QString bodypath = getLicensePath("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/End-User-License-Agreement-Home-CN-%1.txt", "");
        if (QFile::exists(bodypath)) {
            const QString body = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/End-User-License-Agreement-Home-CN-%1.txt", "");
            return body;
        } else {
            const QString oldPody = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt", "");
            return oldPody;
        }
    } else {
        const QString newpath = getLicensePath("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/End-User-License-Agreement-Professional-CN-%1.txt", "");
        if (QFile::exists(newpath)) {
            const QString path = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/End-User-License-Agreement-Professional-CN-%1.txt", "");
            return path;
        } else {
            const QString oldPath = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt", "");
            return oldPath;
        }
    }
}

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

    QFutureWatcher<QString> *w = new QFutureWatcher<QString>(this);
    w->setFuture(QtConcurrent::run(loadLicenses));

    connect(w, &QFutureWatcher<QString>::finished, this, [ = ] {
        const QString r = w->result();
        m_body->setText(r);
    });
}


