// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCFACTORY_H
#define DCCFACTORY_H

#include <QObject>
#include <QVariantMap>

namespace dccV25 {
#define DccFactory_iid "org.deepin.dde.dcc-factory/v2.0"
class DccObject;

class DccFactory_20 : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    // 注册类型，在主线程中调用
    virtual void registerType() { };

    // 作为数据返回，会导出为dccData供main.qml使用，在子线程中调用
    virtual QObject *create(QObject * = nullptr) { return nullptr; }

    // 激活，在主线程中调用
    virtual void active() { };

    // 未提供qml的，可在此自己加载qml返回DccObject对象，在主线程中调用
    virtual DccObject *dccObject(QObject * = nullptr) { return nullptr; }

    //////////^^^^初始化流程按以上函数顺序调用^^^^//////////////////

    // 获取数据，DBus触发，在子线程中调用（注意加锁），建议支持批量获取
    virtual const QVariantMap get(const QVariantMap &param) { return QVariantMap(); }

    // 设置数据，DBus触发，在子线程中调用（注意加锁），建议支持批量设置，返回为空表示设置成功，否则返回错误信息
    virtual const QVariantMap set(const QVariantMap &param) { return QVariantMap(); }

Q_SIGNALS:
    // 模块中有设置项变化时，发出该信号
    // 命名valuesChanged() 待定
    void propertiesChanged(const QVariantMap &properties);
};
} // namespace dccV25
Q_DECLARE_INTERFACE(dccV25::DccFactory_20, DccFactory_iid)

#define DCC_FACTORY_CLASS(classname)                                         \
    namespace {                                                              \
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory_20 \
    {                                                                        \
        Q_OBJECT                                                             \
        Q_PLUGIN_METADATA(IID DccFactory_iid)                                \
        Q_INTERFACES(dccV25::DccFactory_20);                                 \
                                                                             \
    public:                                                                  \
        using dccV25::DccFactory_20::DccFactory_20;                          \
        QObject *create(QObject *parent = nullptr) override                  \
        {                                                                    \
            return new classname(parent);                                    \
        }                                                                    \
    };                                                                       \
    }

#define DCC_FULL_FACTORY_CLASS(classname)                                                                          \
    namespace {                                                                                                    \
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory_20                                       \
    {                                                                                                              \
        Q_OBJECT                                                                                                   \
        Q_PLUGIN_METADATA(IID DccFactory_iid)                                                                      \
        Q_INTERFACES(dccV25::DccFactory_20);                                                                       \
                                                                                                                   \
    public:                                                                                                        \
        using dccV25::DccFactory_20::DccFactory_20;                                                                \
        void registerType() override                                                                               \
        {                                                                                                          \
            classname::registerType();                                                                             \
        }                                                                                                          \
        QObject *create(QObject *parent = nullptr) override                                                        \
        {                                                                                                          \
            if (!m_object) {                                                                                       \
                m_object = new classname(parent);                                                                  \
                connect(m_object, &classname::propertiesChanged, this, &classname##DccFactory::propertiesChanged); \
            }                                                                                                      \
            return m_object;                                                                                       \
        }                                                                                                          \
        void active() override                                                                                     \
        {                                                                                                          \
            m_object->active();                                                                                    \
        }                                                                                                          \
        const QVariantMap get(const QVariantMap &param) override                                                           \
        {                                                                                                          \
            return m_object->get(param);                                                                           \
        }                                                                                                          \
        const QVariantMap set(const QVariantMap &param) override                                                           \
        {                                                                                                          \
            return m_object->set(param);                                                                           \
        }                                                                                                          \
                                                                                                                   \
    private:                                                                                                       \
        classname *m_object = nullptr;                                                                             \
    };                                                                                                             \
    }
#endif // DCCFACTORY_H
