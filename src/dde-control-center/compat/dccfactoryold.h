// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// 私有 v1 工厂定义，仅用于运行期兼容已编译的 v1 插件。
// 该文件不安装到 dev 包，新插件不得包含它。
// 类名、命名空间、继承关系、虚函数顺序与签名必须与旧版公共 dccfactory.h 保持一致，
// 不得向本类插入新虚函数或调整顺序，否则旧插件二进制的 vtable 调用会错位。
#ifndef DCCFACTORYOLD_H
#define DCCFACTORYOLD_H

#include <QObject>

namespace dccV25 {
#define DccFactory_iid_v10 "org.deepin.dde.dcc-factory/v1.0"
class DccObject;

class DccFactory : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    // 作为数据返回，会导出为dccData供main.qml使用
    virtual QObject *create(QObject * = nullptr) { return nullptr; }

    // 未提供qml的，可在此自己加载qml返回DccObject对象
    virtual DccObject *dccObject(QObject * = nullptr) { return nullptr; }
};
} // namespace dccV25
Q_DECLARE_INTERFACE(dccV25::DccFactory, DccFactory_iid_v10)

#endif // DCCFACTORYOLD_H
