// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QSpinBox>

namespace DCC_NAMESPACE {
namespace  datetime{
class TimeSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit TimeSpinBox(QWidget *parent = nullptr);

protected:
     QString textFromValue(int value) const;
};
}
}
