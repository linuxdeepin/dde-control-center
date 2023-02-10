//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include "interface/namespace.h"

#include <QObject>
#include <QMap>

QT_BEGIN_NAMESPACE
class QJsonArray;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class SysItemModel;
class AppItemModel;

class NotificationModel : public QObject
{
    Q_OBJECT
public:
    explicit NotificationModel(QObject *parent = nullptr);
    void setSysSetting(SysItemModel* item);
    inline int getAppSize()const {return m_appItemModels.size();}
    inline SysItemModel *getSystemModel()const {return m_sysItemModel;}
    inline AppItemModel *getAppModel(const int &index) {return m_appItemModels[index];}
    void clearModel();

public Q_SLOTS:
    void appAdded(AppItemModel* item);
    void appRemoved(const QString &appName);

Q_SIGNALS:
    void appListChanged();
    void appListAdded(AppItemModel* item);
    void appListRemoved(AppItemModel* item);

private:
    SysItemModel *m_sysItemModel;
    QList<AppItemModel *> m_appItemModels;
    QString m_theme;
};
}
