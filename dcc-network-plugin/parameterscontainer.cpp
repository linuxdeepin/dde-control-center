// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "parameterscontainer.h"

ParametersContainer::ParametersContainer(QObject *parent)
    : QObject(parent)
{
}

ParametersContainer::~ParametersContainer()
{
}

void ParametersContainer::saveParameters(const ParamType &key, const QVariant &value)
{
    m_pamameters[key] = value;
}

QVariant ParametersContainer::getValue(const ParamType &key)
{
    if (m_pamameters.contains(key))
        return m_pamameters.value(key);

    return QVariant();
}
