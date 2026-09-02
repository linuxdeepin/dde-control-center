// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPointer>

#include "notificationsetting.h"

class QAbstractItemModel;

class AppMgr : public QObject
{
    Q_OBJECT
public:
    static AppMgr *instance();
    explicit AppMgr(QObject *parent = nullptr);
    ~AppMgr() override;

    QList<DCC_NAMESPACE::NotificationSetting::AppItem> allAppInfosShouldBeShown() const;

Q_SIGNALS:
    void appItemAdd(const QString &id);
    void appItemRemove(const QString &id);

private:
    bool createAppItem(int row);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);

    QPointer<QAbstractItemModel> m_appModel;
    QMap<QString, DCC_NAMESPACE::NotificationSetting::AppItem> m_appItems;
    mutable QMutex m_mutex;
};
