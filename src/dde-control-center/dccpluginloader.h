// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "dccfactory.h"

#include <QObject>
#include <QQmlEngine>
#include <QString>

namespace dccV25 {

class DccObject;

class DccPluginManager;

class DccPluginLoader : public QObject
{
    Q_OBJECT

public:
    // Status bit flags for state machine
    enum Status {
        // metaData 0xFF000000
        PluginBegin = 0x10000000,
        PluginEnd = 0x20000000,
        MetaDataLoad = 0x02000000,
        MetaDataEnd = 0x04000000,
        MetaDataErr = 0x08000000,
        // module 0x00FF0000
        ModuleLoad = 0x00010000,
        ModuleCreate = 0x00020000,
        ModuleEnd = 0x00400000,
        ModuleErr = 0x00800000,
        // data 0x0000FF00
        DataBegin = 0x00000100,
        DataLoad = 0x00000200,
        DataEnd = 0x00004000,
        DataErr = 0x00008000,
        // mainObj 0x000000FF
        MainObjLoad = 0x00000001,
        MainObjCreate = 0x00000002,
        MainObjAdd = 0x00000004,
        MainObjEnd = 0x00000040,
        MainObjErr = 0x00000080,

        PluginErrMask = MetaDataErr | ModuleErr | DataErr | MainObjErr,
        PluginEndMask = PluginEnd | MetaDataEnd | ModuleEnd | DataEnd | MainObjEnd,
    };
    Q_DECLARE_FLAGS(StatusFlags, Status)
    Q_FLAG(StatusFlags)

    // Plugin version and capabilities
    enum Type {
        T_Unknown = 0,
        T_V1_0 = 0x01000000,
        T_V1_1 = 0x02000000,
        T_V_MASK = 0xFF000000,

        T_HasModule = 0x00000001,
        T_HasMain = 0x00000002,
        T_DataMask = 0x00000003,
        T_ShortMain = 0x00000004, // main module named main.qml
        T_NoDataSo = 0x00000008,  // 目录中不存在 {name}.so：纯 QML 插件，无 C++ 数据后端
    };
    Q_DECLARE_FLAGS(TypeFlags, Type)
    Q_FLAG(TypeFlags)

    explicit DccPluginLoader(const QString &name, const QString &path, DccPluginManager *manager);
    ~DccPluginLoader() override;

    // Properties
    inline QString name() const { return objectName(); };

    QString path() const;
    StatusFlags status() const;
    TypeFlags type() const;
    DccObject *module() const;
    DccObject *mainObj() const;
    DccObject *soObj() const;
    QObject *data() const;
    // 统一对外接口
    DccFactory_v20 *factory() const;
    bool hasFactory() const;
    // 能力判定：请求层按具体操作（Get/Set）分别检查
    bool supportsCapability(DccCapability capability) const;
    void setLog(const QString &log);
    // 是否有 C++ 数据后端：加载时按是否存在 {name}.so 记录在类型位中
    bool hasDataBackend() const { return !(m_type & T_NoDataSo); }
    // 数据后端的显式状态：请求层据此决定排队、拒绝还是派发，
    // 不再依赖 factory() 是否为空或能力位是否同时具备 Get/Set。
    enum class BackendState {
        NotDiscovered, // 元数据阶段未完成，尚不知道它有没有后端
        Loading,       // 已进入数据阶段，后端尚未激活
        Ready,         // 后端已创建并激活，可服务请求
        Failed,        // 加载或创建失败
        NotSupported,  // 不提供独立数据后端（纯 QML，或 v1 无数据能力）
    };
    BackendState backendState() const { return m_backendState; }
    void setBackendState(BackendState state);
    // 失败原因，供请求层转成 DBus 错误文本
    const QString &backendError() const { return m_backendError; }
    // State query methods
    bool isFinished() const;
    // 数据后端是否加载/创建失败。只看 DataErr：MetaData/Module/MainObj 错误属于
    // 页面与插件包问题，不能用来判定数据后端损坏，否则纯 QML 的页面错误会被
    // 误报成 "backend failed to load"。
    bool hasDataError() const;
    bool isVisibleToApp() const;

    // Dependency injection
    void setType(TypeFlags type);

    // Loading methods
    bool loadMetaData();
    bool loadModule();
    // loadData 在工作线程加载动态库并分派接口版本；
    // 返回 true 表示 v2 插件已将 factory 迁移到主线程，需等 initialized() 后再 createData
    bool loadData();
    bool needsInitialize() const;
    void initializeFactory();  // 主线程：调用 v2 initialized()，只执行一次
    void createData();
    void createDccObject();
    void moveThread();
    void updateParent();
    void loadMain();
    void addMainObject();
    void activate();           // 主线程：调用 v2 active()，只执行一次
    void deactivate();         // 主线程：调用 v2 deactive()，只执行一次

    // Utility
    uint version() const;
    void transitionStatus(StatusFlags status);
private:
    // Private helper methods
    bool updateType();

public Q_SLOTS:
    void updateVisible(bool visibleToApp);

Q_SIGNALS:
    void statusChanged(DccPluginLoader *loader, StatusFlags status);
    // v2 插件在工作线程完成动态库加载并把 factory 迁移到主线程后发出
    void factoryReady(DccPluginLoader *loader);
    // 后端进入终态（Ready/Failed/NotSupported）时发出。同一 loader 可能因隐藏后
    // 恢复可见而重新加载并再次进入终态，因此每次进入终态各发一次，
    // 接收方必须按当前状态处理，不能假设只收到一次。
    void backendStateChanged(DccPluginLoader *loader);

private:
    QString m_path;
    std::atomic<StatusFlags> m_status;
    TypeFlags m_type;

    // v2 为原生工厂（插件库所有，loader 只借用）；v1 为 DccFactoryV1Adapter，
    // 由 loader 独占创建并负责释放。两类所有权必须显式区分，不能靠是否为 nullptr 推断。
    DccFactory_v20 *m_factory;
    bool m_ownsFactory = false;
    DccObject *m_module;
    DccObject *m_mainObj;
    DccObject *m_soObj;
    QObject *m_data;

    // v2 生命周期阶段：initialized 在工作线程读取，其余只在主线程修改
    std::atomic<bool> m_initialized{ false };
    bool m_activated = false;
    bool m_deactivated = false;

    // 后端状态只在主线程修改；由 setBackendState() 统一写入
    BackendState m_backendState = BackendState::NotDiscovered;
    QString m_backendError;

    DccPluginManager *m_pManager;
    QStringList m_log;
};

} // namespace dccV25

Q_DECLARE_METATYPE(dccV25::DccPluginLoader *)
Q_DECLARE_OPERATORS_FOR_FLAGS(dccV25::DccPluginLoader::StatusFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(dccV25::DccPluginLoader::TypeFlags)
