// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/sync/utils.h"
#include "loginedin.h"

#include <dtkwidget_global.h>

#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DTipLabel;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
class QStandardItem;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SwitchWidget;
}

namespace cloudsync {
class SyncModel;
class SyncStateIcon;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class DownloadUrl;
class AvatarWidget;
class IndexPage : public LoginedIn
{
    Q_OBJECT
public:
    IndexPage(QWidget *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel *model);
    ~IndexPage();

Q_SIGNALS:
    void requestSetAutoSync(bool enable) const;
    void requestLogout(const QString &ubid) const;
    void requestSetModuleState(std::pair<dcc::cloudsync::SyncType, bool> state);
    void requestUOSID(QString &uosid);
    void requestUUID(QString &uuid);
    void requestLocalBindCheck(const QString &uosid, const QString &uuid);
    void requestHostName(QString &hostName);
    void requestBindAccount(const QString &uuid, const QString &hostName);
    void requestUnBindAccount(const QString &ubid);
private:
    void onListViewClicked(const QModelIndex &index);
    void onStateChanged(const std::pair<qint32, QString> &state);
    void onLastSyncTimeChanged(const qlonglong lastSyncTime);
    void onModuleStateChanged(std::pair<dcc::cloudsync::SyncType, bool> state);
    void onModuleItemSwitched(const bool checked);
    void onAutoSyncChanged(bool autoSync);
    void updateUserBindStatus();
    void onBindUserAccountChanged(bool checked);
    void bindUserAccount();
    void unbindUserAccount();

public Q_SLOTS:
    void SyncTimeLbl(bool checked);
    void onUbid(const QString &ubid);
    void onResetPasswdError(const QString &error);
    void onUnBindRet(bool ok);
private:
    QVBoxLayout *m_mainLayout;
    dcc::widgets::SwitchWidget *m_bindSwitch;
    dcc::widgets::SwitchWidget *m_autoSyncSwitch;
    DTK_WIDGET_NAMESPACE::DListView *m_listView;
    dcc::cloudsync::SyncStateIcon *m_stateIcon;
    QLabel *m_stateLbl;
    QLabel *m_lastSyncTimeLbl;
    qlonglong m_lastSyncTime;
    QStandardItemModel *m_listModel;
    QMap<dcc::cloudsync::SyncType, QStandardItem *> m_itemMap;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_networkTip;
    std::pair<qint32, QString> m_state;
    QLabel *m_lab;
    QString m_ubid;
};
} // namespace sync
} // namespace DCC_NAMESPACE
