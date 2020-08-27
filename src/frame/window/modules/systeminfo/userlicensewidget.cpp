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
    if (DSysInfo::deepinType() == DSysInfo::DeepinType::DeepinPersonal) {
        const QString body = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/End-User-License-Agreement-Home-CN-%1.txt", "");
        return body;
    } else {
        const QString path = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/End-User-License-Agreement-Professional-CN-%1.txt", "");
        return path;
    }
}

UserLicenseWidget::UserLicenseWidget(QWidget *parent)
    : ContentWidget(parent)
{
    TranslucentFrame *widget = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout();

    m_body = new TipsLabel;
    m_body->setWordWrap(true);

    layout->setContentsMargins(ScrollAreaMargins);
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


