//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QWidget>
#include <QLabel>


namespace DCC_NAMESPACE {

class UserLicenseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserLicenseWidget(QWidget *parent = nullptr);
    void setUserLicense(const QString &text);
Q_SIGNALS:
    void loadTextFinished();

private:
    QLabel *m_body;
};

}
