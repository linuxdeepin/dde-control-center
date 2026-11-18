// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccfactoryv1adapter.h"

#include "compat/dccfactoryold.h"

namespace dccV25 {

DccFactoryV1Adapter::DccFactoryV1Adapter(DccFactory *v1, QObject *parent)
    : DccFactory_v20(parent)
    , m_v1(v1)
{
    // v1 工厂作为适配器子对象，随适配器一起迁移线程
    if (m_v1) {
        m_v1->setParent(this);
    }
}

QObject *DccFactoryV1Adapter::create(QObject *parent)
{
    // 与 v1 时代一致：不传 parent，由框架 moveThread/updateParent 接管
    Q_UNUSED(parent)
    return m_v1 ? m_v1->create() : nullptr;
}

DccObject *DccFactoryV1Adapter::dccObject(QObject *parent)
{
    Q_UNUSED(parent)
    return m_v1 ? m_v1->dccObject() : nullptr;
}

DccCapabilities DccFactoryV1Adapter::capabilities() const
{
    // v1 插件只有数据对象与页面回退，不声明数据通道、变更通知与生命周期
    return DccCapability::DccObjectPage;
}

DccFactory *DccFactoryV1Adapter::v1Factory() const
{
    return m_v1;
}

} // namespace dccV25

#include "dccfactoryv1adapter.moc"
