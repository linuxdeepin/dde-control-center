//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <qdebug.h>
#include <QObject>
#include <QMap>

QT_BEGIN_NAMESPACE
class QJsonArray;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class NotificationSetting;
class SysItemModel;
class AppItemModel;
class NotificationWorker;
class AppsSourceModel;
class AppsListModel;

class NotificationModel : public QObject
{
    Q_OBJECT
public:
    explicit NotificationModel(QObject *parent = nullptr);
    ~NotificationModel();

    Q_PROPERTY(SysItemModel *sysItemModel READ sysItemModel CONSTANT)
    Q_PROPERTY(QList<AppItemModel *> appItemModels READ appItemModels NOTIFY appItemModelsChanged FINAL)
    Q_INVOKABLE AppsListModel *appListModel() {return m_appsListModel;}

    void setSysSetting(SysItemModel* item);
    inline int getAppSize()const {return m_appItemModels.size();}
    inline SysItemModel *getSystemModel()const {return m_sysItemModel;}
    inline AppItemModel *getAppModel(const int &index) {return m_appItemModels[index];}
    void clearModel();


    inline SysItemModel *sysItemModel() const {return m_sysItemModel;}
    QList<AppItemModel *> appItemModels() const {return m_appItemModels;}

public Q_SLOTS:
    void appAdded(AppItemModel* item);
    void appRemoved(const QString &appName);

Q_SIGNALS:
    void appListAdded(AppItemModel* item);
    void appListRemoved(AppItemModel* item);

    void appItemModelsChanged();

private:
    NotificationSetting *m_setting;
    SysItemModel *m_sysItemModel;
    QList<AppItemModel *> m_appItemModels;
    AppsSourceModel *m_appsSourceModel;
    AppsListModel *m_appsListModel;
};
}
