// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QString>

namespace dccV25 {

class DccObject;
class DccFactory;
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
    DccFactory *factory() const;
    void setLog(const QString &log);
    // State query methods
    bool isFinished() const;
    bool hasError() const;
    bool isVisibleToApp() const;

    // Dependency injection
    void setType(TypeFlags type);

    // Loading methods
    void reset(); // Reset status and restart loading
    bool loadMetaData();
    bool loadModule();
    void loadData(); // For async loading in worker thread
    void createData();
    void createDccObject();
    void moveThread();
    void updateParent();
    void loadMain();
    void addMainObject();
    void cancel();

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

private:
    QString m_path;
    std::atomic<StatusFlags> m_status;
    TypeFlags m_type;
    DccFactory *m_factory;
    DccObject *m_module;
    DccObject *m_mainObj;
    DccObject *m_soObj;
    QObject *m_data;

    DccPluginManager *m_pManager;
    QStringList m_log;
};

} // namespace dccV25

Q_DECLARE_METATYPE(dccV25::DccPluginLoader *)
Q_DECLARE_OPERATORS_FOR_FLAGS(dccV25::DccPluginLoader::StatusFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(dccV25::DccPluginLoader::TypeFlags)
