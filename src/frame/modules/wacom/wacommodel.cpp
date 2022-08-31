// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "wacommodel.h"
#include "model/wacommodelbase.h"
#include <QObject>
#include <QDebug>
using namespace dcc;
using namespace dcc::wacom;

WacomModel::WacomModel(QObject *parent)
    : QObject(parent),

      m_exist(false)
{
    m_wacomModelBase = new WacomModelBase;
}

WacomModel::~WacomModel()
{
    m_wacomModelBase->deleteLater();
}

WacomModelBase *WacomModel::getWacomModelBase()
{
    return m_wacomModelBase;
}

bool WacomModel::exist() const
{
    return m_exist;
}

void WacomModel::setExist(bool exist)
{
    if (m_exist != exist) {
        m_exist = exist;
        Q_EMIT existChanged(exist);
    }
}
