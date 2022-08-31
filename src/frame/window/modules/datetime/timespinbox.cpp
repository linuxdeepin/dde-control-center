// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "timespinbox.h"

#include <QLineEdit>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

TimeSpinBox::TimeSpinBox(QWidget *parent)
    : QSpinBox (parent)
{
    this->lineEdit()->setMaxLength(2);
}

QString TimeSpinBox::textFromValue(int value) const
{
    QString str = QString("%1").arg(value, 2, 10, QLatin1Char('0'));
    return str;
}
