// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "dccfactory.h"
#include "dccfactoryold.h"

namespace dccV25 {

// 适配器类：兼容旧版 DccFactory，适配到 DccFactory_20
// 用于 DccPluginLoader 中统一处理新旧版本工厂
class DccFactoryAdapter : public DccFactory_20
{
public:
    // 包装旧版 DccFactory
    explicit DccFactoryAdapter(DccFactory *oldFactory, QObject *parent = nullptr)
        : DccFactory_20(parent)
        , m_oldFactory(oldFactory)
    {
    }

    // 旧工厂不存在时的安全检查
    bool hasOldFactory() const { return m_oldFactory != nullptr; }

    // ===== DccFactory_20 接口实现 =====

    // 注册类型（转发给旧工厂）
    void registerType() override
    {
        if (m_oldFactory) {
            // 旧工厂没有 registerType，但可能有 qmlRegisterType 等调用
            // 这里可以添加兼容性处理
        }
    }

    // 创建数据对象（转发）
    QObject *create(QObject *parent = nullptr) override
    {
        if (m_oldFactory) {
            return m_oldFactory->create(parent);
        }
        return nullptr;
    }

    // 激活（转发给数据对象）
    void active() override
    {
        if (m_data) {
            // 通知数据对象激活
            QMetaObject::invokeMethod(m_data, "active", Qt::AutoConnection);
        }
    }

    // 创建 DccObject（转发）
    DccObject *dccObject(QObject *parent = nullptr) override
    {
        if (m_oldFactory) {
            return m_oldFactory->dccObject(parent);
        }
        return nullptr;
    }

    // 获取数据（新版接口，旧工厂不支持，返回空）
    const QVariantMap get(const QVariantMap &param) override
    {
        Q_UNUSED(param)
        // 旧工厂不支持 get，返回空结果
        return QVariantMap();
    }

    // 设置数据（新版接口，旧工厂不支持，返回空）
    const QVariantMap set(const QVariantMap &param) override
    {
        Q_UNUSED(param)
        // 旧工厂不支持 set，返回空结果
        return QVariantMap();
    }

    // ===== 数据对象管理 =====

    // 设置数据对象（用于 active 调用）
    void setDataObject(QObject *data) { m_data = data; }

    QObject *dataObject() const { return m_data; }

private:
    DccFactory *m_oldFactory;  // 包装的旧工厂
    QObject *m_data = nullptr; // 创建的数据对象
};

} // namespace dccV25
