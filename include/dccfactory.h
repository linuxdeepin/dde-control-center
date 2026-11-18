// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCFACTORY_H
#define DCCFACTORY_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QVariantMap>

namespace dccV25 {
#define DccFactory_iid "org.deepin.dde.dcc-factory/v2.0"
class DccObject;

// 数据通道结果。框架与插件用状态区分“未实现”“未就绪”和“成功”，
// 不能用空字符串同时表达这三种含义。
enum class DataStatus : quint8 {
    Ok = 0,
    NotSupported = 1,
    InvalidArgs = 2,
    NotReady = 3,
    AccessDenied = 4,
    Failed = 5,
    NotFound = 6, // 模块不存在：与参数错误、后端失败区分开
};

struct DataReply
{
    DataStatus status = DataStatus::NotSupported;
    // Get 返回请求键到结果的映射；Set 返回每项的逐项结果（空值表示成功）
    QVariantMap value;
    QString message;
};
// 插件工厂支持的能力位。框架用 capabilities() 判定某能力是否可用，
// 而不是依赖指针类型或空返回值。
enum class DccCapability : quint8 {
    None = 0,
    Get = 1 << 0,               // 数据通道：get()
    Set = 1 << 1,               // 数据通道：set()
    PropertiesChanged = 1 << 2, // 变更通知：propertiesChanged
    Lifecycle = 1 << 3,         // initialized/active/deactive 生命周期钩子
    DccObjectPage = 1 << 4,     // dccObject() 页面回退
};
Q_DECLARE_FLAGS(DccCapabilities, DccCapability)

class DccFactory_v20 : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    ~DccFactory_v20() override = default;

    // 工厂声明自身支持的能力。
    // 默认只声明生命周期；dccObject()/get/set/propertiesChanged 需要派生类显式声明。
    virtual DccCapabilities capabilities() const { return DccCapability::Lifecycle; }

    // 在主线程中调用，保证早于 create()，可在此注册 qml 类型
    virtual void initialized() { }

    // 作为数据返回，会导出为 dccData 供 main.qml 使用，在子线程中调用
    // 返回的 QObject 顶层对象必须未设置跨线程 parent，由框架迁移到主线程后接管
    virtual QObject *create(QObject * = nullptr) { return nullptr; }

    // 激活，在主线程中调用，位于数据迁移到主线程之后
    virtual void active() { }

    // 停用，在主线程中调用，位于数据对象释放之前
    virtual void deactive() { }

    // 未提供 qml 的，可在此自己加载 qml 返回 DccObject 对象，在主线程中调用
    virtual DccObject *dccObject(QObject * = nullptr) { return nullptr; }

    // DBus 数据通道，在框架的数据工作线程中调用
    // param 为模块定义的键值映射
    // caller 为真实 DBus 调用者唯一总线名（如 ":1.42"），不接受请求体自称的身份
    // callerPath 为调用者可执行文件的真实路径，由框架按总线名解析；
    // 无法解析时为空串——插件须把空串当作"未知"，不能用其它字段补齐后当真
    virtual DataReply get(const QVariantMap &, const QString &caller,
                          const QString &callerPath)
    {
        Q_UNUSED(caller)
        Q_UNUSED(callerPath)
        return {};
    }

    virtual DataReply set(const QVariantMap &, const QString &caller,
                          const QString &callerPath)
    {
        Q_UNUSED(caller)
        Q_UNUSED(callerPath)
        return {};
    }

Q_SIGNALS:
    // 模块中有设置项变化时发出，由框架转成带模块名的 DBus 信号
    void propertiesChanged(const QVariantMap &properties);
};
} // namespace dccV25
Q_DECLARE_OPERATORS_FOR_FLAGS(dccV25::DccCapabilities)
Q_DECLARE_INTERFACE(dccV25::DccFactory_v20, DccFactory_iid)

// 基础接入：每次 create() 新建后端对象。
// 能力与基类默认一致（Lifecycle），无需重写 capabilities()。
#define DCC_FACTORY_CLASS(classname)                                          \
    namespace {                                                               \
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory_v20 \
    {                                                                         \
        Q_OBJECT                                                              \
        Q_PLUGIN_METADATA(IID DccFactory_iid)                                 \
        Q_INTERFACES(dccV25::DccFactory_v20)                                  \
    public:                                                                   \
        using dccV25::DccFactory_v20::DccFactory_v20;                         \
        QObject *create(QObject *parent = nullptr) override                   \
        {                                                                     \
            return new classname(parent);                                     \
        }                                                                     \
    };                                                                        \
    }

// 完整接入：缓存后端对象并转发 initialized/active/deactive/get/set 及变更信号。
// 声明全部能力；后端是否真的实现由模块自身保证。
// 后端需要提供 static void initialized()、active()、deactive()、get()、set()
// 以及 propertiesChanged(const QVariantMap &) 信号
#define DCC_FULL_FACTORY_CLASS(classname)                                                                                                                                          \
    namespace {                                                                                                                                                                    \
    class Q_DECL_EXPORT classname##DccFactory : public dccV25::DccFactory_v20                                                                                                      \
    {                                                                                                                                                                              \
        Q_OBJECT                                                                                                                                                                   \
        Q_PLUGIN_METADATA(IID DccFactory_iid)                                                                                                                                      \
        Q_INTERFACES(dccV25::DccFactory_v20)                                                                                                                                       \
    public:                                                                                                                                                                        \
        using dccV25::DccFactory_v20::DccFactory_v20;                                                                                                                              \
        dccV25::DccCapabilities capabilities() const override                                                                                                                      \
        {                                                                                                                                                                          \
            return dccV25::DccCapabilities(dccV25::DccCapability::Lifecycle) | dccV25::DccCapability::Get | dccV25::DccCapability::Set | dccV25::DccCapability::PropertiesChanged; \
        }                                                                                                                                                                          \
        void initialized() override                                                                                                                                                \
        {                                                                                                                                                                          \
            classname::initialized();                                                                                                                                              \
        }                                                                                                                                                                          \
        QObject *create(QObject *parent = nullptr) override                                                                                                                        \
        {                                                                                                                                                                          \
            if (m_object) {                                                                                                                                                        \
                return m_object;                                                                                                                                                   \
            }                                                                                                                                                                      \
            m_object = new classname(parent);                                                                                                                                      \
            connect(m_object, &classname::propertiesChanged, this, &classname##DccFactory::propertiesChanged);                                                                     \
            return m_object;                                                                                                                                                       \
        }                                                                                                                                                                          \
        void active() override                                                                                                                                                     \
        {                                                                                                                                                                          \
            if (m_object) {                                                                                                                                                        \
                m_object->active();                                                                                                                                                \
            }                                                                                                                                                                      \
        }                                                                                                                                                                          \
        void deactive() override                                                                                                                                                   \
        {                                                                                                                                                                          \
            if (m_object) {                                                                                                                                                        \
                m_object->deactive();                                                                                                                                              \
            }                                                                                                                                                                      \
        }                                                                                                                                                                          \
        dccV25::DataReply get(const QVariantMap &param, const QString &caller, const QString &callerPath) override                                                             \
        {                                                                                                                                                                          \
            if (!m_object) {                                                                                                                                                       \
                return { dccV25::DataStatus::NotReady, {}, QStringLiteral("backend not created") };                                                                                \
            }                                                                                                                                                                      \
            return m_object->get(param, caller, callerPath);                                                                                                                        \
        }                                                                                                                                                                          \
        dccV25::DataReply set(const QVariantMap &param, const QString &caller, const QString &callerPath) override                                                             \
        {                                                                                                                                                                          \
            if (!m_object) {                                                                                                                                                       \
                return { dccV25::DataStatus::NotReady, {}, QStringLiteral("backend not created") };                                                                                \
            }                                                                                                                                                                      \
            return m_object->set(param, caller, callerPath);                                                                                                                        \
        }                                                                                                                                                                          \
    private:                                                                                                                                                                       \
        QPointer<classname> m_object;                                                                                                                                              \
    };                                                                                                                                                                             \
    }
#endif // DCCFACTORY_H
