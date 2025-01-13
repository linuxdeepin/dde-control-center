// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccfactory.h"
#include "deepinidinterface.h"
#include "utils.h"

DeepinIDInterface::DeepinIDInterface(QObject *parent)
    : QObject(parent)
    , m_model(new DeepinidModel(this))
    , m_worker(new DeepinWorker(m_model, this))
{
    m_worker->initData();
}

QString DeepinIDInterface::editionName() const
{
    if (IsCommunitySystem) {
        return tr("deepin");
    } else {
        return tr("UOS");
    }
}

DCC_FACTORY_CLASS(DeepinIDInterface)

#include "deepinidinterface.moc"
