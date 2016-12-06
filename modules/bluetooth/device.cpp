/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "device.h"

namespace dcc {
namespace bluetooth {

Device::Device(QObject *parent) :
    QObject(parent)
{

}

void Device::setId(const QString &id)
{
    m_id = id;
}

void Device::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void Device::setPaired(bool paired)
{
    if (paired != m_paired) {
        m_paired = paired;
        emit pairedChanged(paired);
    }
}

} // namespace bluetooth
} // namespace dcc
