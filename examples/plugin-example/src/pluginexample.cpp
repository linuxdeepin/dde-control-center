// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "pluginexample.h"

#include "dccfactory.h"

PluginExample::PluginExample(QObject *parent)
    : QObject(parent)
    , m_calcType(0)
{
}

QString PluginExample::name() const
{
    return m_name;
}

void PluginExample::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged(m_name);
    }
}

int PluginExample::calc(int a, int b)
{
    switch (m_calcType) {
    case 0: // +
        return a + b;
        break;
    case 1: // -
        return a - b;
        break;
    case 2: // *
        return a * b;
        break;
    case 3: // /
        return a / b;
        break;
    default:
        return a + b;
        break;
    }
}

void PluginExample::setCalcType(int type)
{
    if (m_calcType != type) {
        m_calcType = type;
        Q_EMIT calcTypeChanged(m_calcType);
    }
}
DCC_FACTORY_CLASS(PluginExample)// DCC_FACTORY_CLASS在dccfactory.h中定义，用于注册插件,该宏会自动生成PluginExampleFactory类，并实现create函数。PluginExampleFactory类为Qt类，所以需要包含pluginexample.moc
#include "pluginexample.moc"
