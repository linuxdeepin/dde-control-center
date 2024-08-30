//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "privacysecurityworker.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <QFutureWatcher>

#include <widgets/servicecontrolitems.h>

PrivacySecurityWorker::PrivacySecurityWorker(PrivacySecurityModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_privacyDBusInter(new PrivacySecurityDBusProxy(this))
{
    connect(m_privacyDBusInter, &PrivacySecurityDBusProxy::permissionInfoLoadFinished, this, &PrivacySecurityWorker::permissionInfoLoadFinished);
    connect(m_privacyDBusInter, &PrivacySecurityDBusProxy::PermissionEnableChanged, this, &PrivacySecurityWorker::refreshPermissionState);
    connect(m_privacyDBusInter, &PrivacySecurityDBusProxy::permissionEnableReset, this, &PrivacySecurityWorker::resetPermissionState);
    connect(m_privacyDBusInter, &PrivacySecurityDBusProxy::permissionInfoReset, this, &PrivacySecurityWorker::resetPermissionInfo);
}

PrivacySecurityWorker::~PrivacySecurityWorker()
{

}

void PrivacySecurityWorker::activate()
{
    m_privacyDBusInter->getPermissionInfo();
}

void PrivacySecurityWorker::deactivate()
{

}

void PrivacySecurityWorker::permissionInfoLoadFinished(const QString &perInfo)
{
    qDebug() << " perInfo : " << perInfo;
    // 解析JSON
    QJsonDocument doc = QJsonDocument::fromJson(perInfo.toUtf8());
    QJsonArray groupDate = doc.array();

    if (groupDate.isEmpty()) {
        // 若为空 清空所有数据
        m_model->clearServiceItemDate();
        return;
    }

    // TODO: 目前只保留一个组 后期会拓展
    QJsonValue valueDate = groupDate.at(0);
    QJsonObject groupCards = valueDate.toObject();

    const QString group = static_cast<QString>(groupCards["group"].toString());

    QStringList permissionDevice;

    // 获取设备权限信息
    QJsonArray permissionInfoDate = groupCards["permissionInfo"].toArray();
    qDebug() << permissionInfoDate.size();
    for (QJsonValue perInfo : permissionInfoDate) {
        QJsonObject perInfoObj = perInfo.toObject();

        const QString permission = perInfoObj["permission"].toString();
        if (!m_model->getServiceVategory().keys().contains(permission))
            continue;

        if (!permissionDevice.contains(permission))
            permissionDevice.append(permission);

        QJsonArray appInfoDate = perInfoObj["appInfo"].toArray();
        qDebug() << "get Apps count： " << appInfoDate.count();

        saveServiceApps(group, permission, appInfoDate);
    }
}

void PrivacySecurityWorker::saveServiceApps(const QString &currentGroup, const QString &dameonDefineName, const QJsonArray &appInfoDate)
{
    // 根据后端定义的名称 转换获取SerivceItem保存数据
    ServiceControlItems *serviceItem = m_model->getServiceItem(dameonDefineName);
    serviceItem->setServiceGroup(currentGroup);
    getPermissionEnable(currentGroup, dameonDefineName);
    serviceItem->setServiceAvailable(!appInfoDate.isEmpty());
    if (!serviceItem) {
        qDebug() << dameonDefineName << "non-existent";
        return;
    }

    QList<App> tmpApss;
    for (QJsonValue appInfo : appInfoDate) {
        QJsonObject appInfoObj = appInfo.toObject();

        const QString &appName = appInfoObj["appName"].toString();
        const QString &enable = appInfoObj["value"].toString();
        App app;
        app.m_name = appName;
        app.m_enable = enable;
        qDebug() << " Add Apps: " << appName << enable;
        tmpApss.append(app);
    }
    serviceItem->setServiceApps(tmpApss);
}

void PrivacySecurityWorker::refreshPermissionState(const QString &permissionGroup, const QString &permissionId, bool enable)
{
    Q_UNUSED(permissionGroup);
    ServiceControlItems *serviceItem = m_model->getServiceItem(permissionId);
    serviceItem->setSwitchState(enable);
}

void PrivacySecurityWorker::resetPermissionState(const QString &permissionGroup, const QString &permissionId)
{
    Q_UNUSED(permissionGroup);
    ServiceControlItems *serviceItem = m_model->getServiceItem(permissionId);
    qDebug() << "serviceItem->getSwitchState(): " << serviceItem->getSwitchState();
    serviceItem->serviceSwitchStateChange(serviceItem->getSwitchState());
}

void PrivacySecurityWorker::resetPermissionInfo(const QString &permissionGroup, const QString &permissionId)
{
    Q_UNUSED(permissionGroup);
    ServiceControlItems *serviceItem = m_model->getServiceItem(permissionId);
    serviceItem->serviceAppsDateChange();
}

const QString PrivacySecurityWorker::getIconPath(const QString &appName)
{
    // TODO: 异步获取 IconPath
    return nullptr;
}

void PrivacySecurityWorker::getPermissionEnable(const QString &permissionGroup, const QString &permissionId)
{
    disconnect(m_privacyDBusInter, &PrivacySecurityDBusProxy::permissionEnableLoadFinished, this, nullptr);
    connect(m_privacyDBusInter, &PrivacySecurityDBusProxy::permissionEnableLoadFinished, this, [=](bool permissionEnable){
        qDebug() << "getPermissionEnable : " << permissionEnable;
        refreshPermissionState(permissionGroup, permissionId, permissionEnable);
    });
    m_privacyDBusInter->getPermissionEnable(permissionGroup, permissionId);
}

void PrivacySecurityWorker::setPermissionEnable(const QString &permissionGroup, const QString &permissionId, bool enable)
{
    m_privacyDBusInter->setPermissionEnable(permissionGroup, permissionId, enable);
}

void PrivacySecurityWorker::setPermissionInfo(const QString &appId, const QString &permissionGroup, const QString &permissionId, const QString &value)
{
    m_privacyDBusInter->setPermissionInfo(appId, permissionGroup, permissionId, value);
}

