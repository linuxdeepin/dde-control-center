// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <QWidget>

namespace  dcc {
namespace widgets {
class TipsLabel;
}
}

namespace DCC_NAMESPACE {
namespace systeminfo {

class UserLicenseWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit UserLicenseWidget(QWidget *parent = nullptr);

Q_SIGNALS:
    void loadTextFinished();

private:
    dcc::widgets::TipsLabel *m_body;
};

}
}
