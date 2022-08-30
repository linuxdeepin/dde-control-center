// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "displaywidget.h"
#include "modules/display/displaymodel.h"

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

DisplayWidget::DisplayWidget(QWidget *parent)
    : dcc::ContentWidget(parent)
{
    setObjectName("Display");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setContentsMargins(0, 0, 0, 0);
}

void DisplayWidget::setModel()
{
}

int DisplayWidget::showPath(const QString &path)
{
    if (path == "Resolution" || path == "Refresh Rate") {
        return 0;
    }

    return -1;
}
