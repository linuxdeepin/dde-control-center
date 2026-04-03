// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCFACTORY_H
#define DCCFACTORY_H

#include <QObject>

namespace dccV25 {
#define DccFactory_iid "org.deepin.dde.dcc-factory/v1.0"
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

#define DccFactoryV2_iid "org.deepin.dde.dcc-factory/v2.0"
class DccFactoryV2
{
public:
    virtual ~DccFactoryV2() = default;

    // 工厂初始化完成回调, 可在此进行一些qml注册等操作，保证在create之前调用
    virtual void initialized() {}
};

} // namespace dccV25
Q_DECLARE_INTERFACE(dccV25::DccFactory, DccFactory_iid)

#define DCC_FACTORY_CLASS(classname)                                      \
    namespace {                                                           \
    static void (*dcc_factory_init_func_)() = nullptr;                    \
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory, public dccV25::DccFactoryV2 \
    {                                                                     \
        Q_OBJECT                                                          \
        Q_PLUGIN_METADATA(IID DccFactory_iid)                             \
        Q_INTERFACES(dccV25::DccFactory)              \
    public:                                                               \
        using dccV25::DccFactory::DccFactory;                             \
        QObject *create(QObject *parent = nullptr) override               \
        {                                                                 \
            return new classname(parent);                                 \
        }                                                                 \
        void initialized() override                                       \
        {                                                                 \
            if (dcc_factory_init_func_)                                    \
                dcc_factory_init_func_();                                  \
        }                                                                 \
    };                                                                    \
    }

#define DCC_FACTORY_INITIALIZED(initFunc)                                 \
    namespace {                                                           \
    static const bool dcc_factory_init_registered_ =                      \
        (dcc_factory_init_func_ = (initFunc), true);                      \
    }
#endif // DCCFACTORY_H
