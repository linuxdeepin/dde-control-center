// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"

#include <QComboBox>
#include <QMouseEvent>

namespace DCC_NAMESPACE {
namespace datetime {

class datetimeCombox : public QComboBox
{
    Q_OBJECT
public:
    datetimeCombox(QWidget *parent = nullptr);

Q_SIGNALS:
    void click();

private:
    virtual void mousePressEvent(QMouseEvent *event) override;
};

}
}
