// SPDX-FileCopyrightText: 2015 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dconfig_helper.h"

#include <QDebug>
#include <QMetaMethod>
#include <QApplication>

DCORE_USE_NAMESPACE

Q_GLOBAL_STATIC(DConfigHelper, dConfigWatcher)

DConfigHelper::DConfigHelper(QObject *parent)
    : QObject(parent)
{
    moveToThread(qApp->thread());
}

DConfigHelper *DConfigHelper::instance()
{
    return dConfigWatcher;
}

DConfig *DConfigHelper::initializeDConfig(const QString &appId,
                                          const QString &name,
                                          const QString &subpath)
{
    QMutexLocker locker(&m_mutex);
    DConfig *dConfig = DConfig::create(appId, name, subpath);
    if (!dConfig) {
        qWarning() << "Create dconfig failed, app id: " << appId << ", name: " << name
                   << ", subpath: " << subpath;
        return nullptr;
    }

    m_dConfigs[packageDConfigPath(appId, name, subpath)] = dConfig;
    m_bindInfos[dConfig] = {};

    dConfig->moveToThread(qApp->thread());
    dConfig->setParent(this);

    // 即时响应数据变化
    connect(dConfig, &DConfig::valueChanged, this, [this, dConfig](const QString &key) {
        const QVariant &value = dConfig->value(key);
        auto it = m_bindInfos.find(dConfig);
        if (it == m_bindInfos.end())
            return;

        auto itBindInfo = it.value().begin();
        for (; itBindInfo != it.value().end(); ++itBindInfo) {
            if (itBindInfo.value().contains(key)) {
                auto callbackIt = m_objCallbackMap.find(itBindInfo.key());
                if (callbackIt != m_objCallbackMap.end())
                    callbackIt.value()(key, value, itBindInfo.key());
            }
        }
    });

    return dConfig;
}

void DConfigHelper::bind(const QString &appId,
                         const QString &name,
                         const QString &subpath,
                         QObject *obj,
                         const QString &key,
                         const OnPropertyChangedCallback &callback)
{
    if (!obj)
        return;

    DConfig *dConfig = dConfigObject(appId, name, subpath);
    if (!dConfig) {
        qWarning() << "Bind failed, dconfig is nullptr, app id: " << appId << ", name: " << name << ", subpath: " << subpath;
        return;
    }

    auto it = m_bindInfos.find(dConfig);
    if (it == m_bindInfos.end()) {
        qWarning() << "Can not find bind info";
        return;
    }

    QMap<QObject *, QStringList> &bindInfo = it.value();
    auto bindInfoIt = bindInfo.find(obj);
    if (bindInfoIt != bindInfo.end()) {
        if (!bindInfoIt.value().contains(key))
            bindInfoIt.value().append(key);
    } else {
        bindInfo[obj] = QStringList(key);
    }

    m_objCallbackMap.insert(obj, callback);
    connect(obj, &QObject::destroyed, this, [this, obj] {
        unBind(obj);
    });
}

void DConfigHelper::bind(const QString& encodedPath,
        QObject* obj,
        const QString& key,
        const OnPropertyChangedCallback &callback)
{
    const QStringList &tmp = encodedPath.split(",");
    if (tmp.size() != 3) {
        qWarning() << "Encoded path is invalid, encoded path: " << encodedPath << ", size: " << tmp.size();
        return;
    }

    bind(tmp[0], tmp[1], tmp[2], obj, key, callback);
}

void DConfigHelper::unBind(QObject *obj, const QString &key)
{
    qDebug() << "DConfig unbind, object: " << obj << ", key: " << key;
    if (!obj) {
        qWarning() << "Unbinding object is null";
        return;
    }

    bool objStillUseful = false;
    auto it = m_bindInfos.begin();
    for (; it != m_bindInfos.end(); ++it) {
        if (key.isEmpty()) {
            it->remove(obj);
        } else {
            // 移除key，移除完如果obj没有绑定了key了，那么把obj也移除掉
            auto it1 = it.value().find(obj);
            if (it1 != it.value().end()) {
                it1.value().removeAll(key);
                if (it1.value().isEmpty()) {
                    it->remove(obj);
                } else {
                    objStillUseful = true;
                }
            }
        }
    }

    if (key.isEmpty() || !objStillUseful)
        m_objCallbackMap.remove(obj);
}

QVariant DConfigHelper::getConfig(const QString &appId,
                                  const QString &name,
                                  const QString &subpath,
                                  const QString &key,
                                  const QVariant &defaultValue)
{
    DConfig *dConfig = dConfigObject(appId, name, subpath);
    if (!dConfig) {
        qWarning() << "Get config failed, dconfig object is null";
        return defaultValue;
    }

    if (!dConfig->keyList().contains(key))
        return defaultValue;

    const QVariant &value = dConfig->value(key);
    return value;
}

QVariant DConfigHelper::getConfig(const QString& encodedPath,
        const QString& key,
        const QVariant& defaultValue)
{
    const QStringList &tmp = encodedPath.split(",");
    if (tmp.size() != 3) {
        qWarning() << "Get config failed, encoded path is invalid:" << encodedPath;
        return defaultValue;
    }

    return getConfig(tmp[0], tmp[1], tmp[2], key, defaultValue);
}

void DConfigHelper::setConfig(const QString &appId,
                              const QString &name,
                              const QString &subpath,
                              const QString &key,
                              const QVariant &value)
{
    DConfig *dConfig = dConfigObject(appId, name, subpath);
    if (!dConfig) {
        qWarning() << "Set config failed, dconfig object is null";
        return;
    }

    if (!dConfig->keyList().contains(key)) {
        qWarning() << "Set config failed, dconfig does not contain key: " << key;
        return;
    }

    dConfig->setValue(key, value);
}

void DConfigHelper::setConfig(const QString& encodedPath,
        const QString& key,
        const QVariant& value)
{
    const QStringList &tmp = encodedPath.split(",");
    if (tmp.size() != 3) {
        qWarning() << "Set config failed, encoded path is invalid:" << encodedPath;
        return;
    }

    return setConfig(tmp[0], tmp[1], tmp[2], key, value);
}

DConfig *DConfigHelper::dConfigObject(const QString &appId,
                                      const QString &name,
                                      const QString &subpath)
{
    const QString &configPath = packageDConfigPath(appId, name, subpath);
    DConfig *dConfig = nullptr;
    if (m_dConfigs.contains(configPath))
        dConfig = m_dConfigs.value(configPath);
    else
        dConfig = initializeDConfig(appId, name, subpath);

    return dConfig;
}

QString DConfigHelper::packageDConfigPath(const QString &appId,
                                          const QString &name,
                                          const QString &subpath) const
{
    return appId + name + subpath;
}

