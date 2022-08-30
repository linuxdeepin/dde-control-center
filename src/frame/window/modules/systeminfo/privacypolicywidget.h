// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <QWidget>

namespace DCC_NAMESPACE {
namespace systeminfo {

class PrivacyPolicyWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit PrivacyPolicyWidget(QWidget *parent = nullptr);

public Q_SLOTS:
    void onLinkActivated(const QString link);
};

}
}
