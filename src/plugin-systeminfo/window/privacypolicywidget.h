//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QWidget>

namespace DCC_NAMESPACE {

class PrivacyPolicyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrivacyPolicyWidget(QWidget *parent = nullptr);

public Q_SLOTS:
    void onLinkActivated(const QString link);
};

}
