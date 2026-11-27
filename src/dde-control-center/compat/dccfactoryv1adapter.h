// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 把已编译的 v1 插件工厂适配成 v2 接口，使 loader 对外只有 DccFactory_v20。
// 仅转发 v1 实际具备的能力：create()/dccObject()。
// initialized/active/deactive 为空实现（v1 插件在构造函数中自行完成初始化），
// get/set 保持基类默认 NotSupported，propertiesChanged 永不发出。
// 该文件不安装到 dev 包，仅用于框架内运行期兼容。
#ifndef DCCFACTORYV1ADAPTER_H
#define DCCFACTORYV1ADAPTER_H

#include "dccfactory.h"

namespace dccV25 {

class DccFactory;

class DccFactoryV1Adapter final : public DccFactory_v20
{
    Q_OBJECT
public:
    explicit DccFactoryV1Adapter(DccFactory *v1, QObject *parent = nullptr);

    // v1 插件只有数据对象与页面回退；生命周期钩子在适配器内为空实现，
    // 数据通道与变更通知完全不支持
    DccCapabilities capabilities() const override;

    QObject *create(QObject *parent = nullptr) override;
    DccObject *dccObject(QObject *parent = nullptr) override;

    DccFactory *v1Factory() const;

private:
    DccFactory *m_v1;
};

} // namespace dccV25

#endif // DCCFACTORYV1ADAPTER_H
