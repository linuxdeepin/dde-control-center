// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <DConfig>

#include <QMap>
#include <QMutex>
#include <QObject>

/**
 * @brief 配置变化回调函数
 *
 * @param const QString & 配置键值
 * @param const QVariant & 配置值
 * @param QObject * 绑定对象
 */
 
typedef std::function<void(const QString&, const QVariant&, QObject*)> OnPropertyChangedCallback;

class DConfigHelper : public QObject {
    Q_OBJECT
public:
    explicit DConfigHelper(QObject* parent = nullptr);
    static DConfigHelper* instance();

    // 注意：多个配置文件区分同名的key
    void bind(const QString& appId,
        const QString& name,
        const QString& subpath,
        QObject* obj,
        const QString& key,
        const OnPropertyChangedCallback &callback);
    // encodedPath 是将appId name subPath 组合起来，以逗号分割;如果subPath为空，则以逗号结尾
    // eg: org.deepin.dde.dock,org.deepin.dde.dock.plugin.power,
    void bind(const QString& encodedPath,
        QObject* obj,
        const QString& key,
        const OnPropertyChangedCallback &callback);
    void unBind(QObject* obj, const QString& key = "");

    QVariant getConfig(const QString& appId,
        const QString& name,
        const QString& subpath,
        const QString& key,
        const QVariant& defaultValue);
    QVariant getConfig(const QString& encodedPath,
        const QString& key,
        const QVariant& defaultValue);

    void setConfig(const QString& appId,
        const QString& name,
        const QString& subpath,
        const QString& key,
        const QVariant& value);
    void setConfig(const QString& encodedPath,
        const QString& key,
        const QVariant& value);

private:
    Q_DISABLE_COPY(DConfigHelper)

    Dtk::Core::DConfig* initializeDConfig(const QString& appId,
        const QString& name,
        const QString& subpath);
    Dtk::Core::DConfig* dConfigObject(const QString& appId,
        const QString& name,
        const QString& subpath);
    QString packageDConfigPath(const QString& appId,
        const QString& name,
        const QString& subpath) const;
    QString decodeDConfigPath(const QString& path) const;

private:
    QMutex m_mutex;
    QMap<QString, Dtk::Core::DConfig*> m_dConfigs;
    QMap<Dtk::Core::DConfig*, QMap<QObject*, QStringList>> m_bindInfos;
    QMap<QObject*, OnPropertyChangedCallback> m_objCallbackMap;
};
