// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wacommodelbase.h"

using namespace dcc;
using namespace dcc::wacom;

WacomModelBase::WacomModelBase(QObject *parent)
{
    Q_UNUSED(parent);
}

void WacomModelBase::setPressureValue(const int &value)
{
    m_pressureValue = value;
    Q_EMIT pressureValueChanged(value);
}
