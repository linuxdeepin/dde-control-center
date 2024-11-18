// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
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
} // namespace dccV25
Q_DECLARE_INTERFACE(dccV25::DccFactory, DccFactory_iid)

#define DCC_FACTORY_CLASS(classname)                                      \
    namespace {                                                           \
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory \
    {                                                                     \
        Q_OBJECT                                                          \
        Q_PLUGIN_METADATA(IID DccFactory_iid)                             \
        Q_INTERFACES(dccV25::DccFactory)                                  \
    public:                                                               \
        using dccV25::DccFactory::DccFactory;                             \
        QObject *create(QObject *parent = nullptr) override               \
        {                                                                 \
            return new classname(parent);                                 \
        }                                                                 \
    };                                                                    \
    }
#endif // DCCFACTORY_H
