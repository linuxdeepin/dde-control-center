/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "interface/namespace.h"
#include "loginedin.h"
#include "modules/sync/utils.h"

#include <dtkwidget_global.h>

#include <QWidget>
#include <QCheckBox>
#include <DSwitchButton>
#include <DGuiApplicationHelper>
#include <DCommandLinkButton>

DWIDGET_USE_NAMESPACE
using namespace dcc;
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

namespace unionid {
class UnionidModel;
}
}

namespace DCC_NAMESPACE {
namespace unionid {
class DownloadUrl;
class AvatarWidget;
class IndexPage : public LoginedIn
{
    Q_OBJECT
public:
    IndexPage(QWidget *parent = nullptr);
    ~IndexPage();
    void setModel(dcc::unionid::UnionidModel *model);
    void setUserInfo(QString usrInfo);

Q_SIGNALS:
    void requestSetAutoSync(bool enable) const;
    void requestLogout() const;
    void requesUserDialog(QString) const;
    void requestSetModuleState(std::pair<dcc::cloudsync::SyncType, bool> state);

public Q_SLOTS:
    void onRefreshUserInfo(QString usrInfo);

private Q_SLOTS:
    void onStateChanged(const std::pair<qint32, QString> &state);
    void onModuleStateChanged(std::pair<dcc::cloudsync::SyncType, bool> state);
    void onChecked(bool bIschecked);
    void onSwitchButtoncheckedChanged(bool bIschecked);
    void onModifyInfo();
    void onThemeTypeChanged(Dtk::Gui::DGuiApplicationHelper::ColorType themeType);
    void onCheckboxStateChanged(bool state, dcc::cloudsync::SyncType syncType);
    void onModButtonClicked();
    void onGetUserInfoResult();

private:
    QVBoxLayout *m_mainLayout;
    DSwitchButton *m_autoSyncSwitch;
//    dcc::widgets::SwitchWidget *m_autoSyncSwitch;
    QMap<dcc::cloudsync::SyncType, QCheckBox *> m_itemMap;
    DTK_WIDGET_NAMESPACE::DListView *m_listView;
    QStandardItemModel *m_listModel;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_networkTip;
    QLabel *m_logout;
    QLabel *m_lab;
    QVBoxLayout *m_syncLayout;
    QWidget *m_syncWidget;
    QLabel *m_nameLabel;
    QLabel *m_uidLabel;
    QLabel *m_wxlabel;
    QLabel *m_wxNameLabel;
    QLabel *m_autoSync;
    AvatarWidget *m_userAvatar;
    QString m_accessToken;
    QString m_refreshToken;
    QString m_phoneNumber;
    QString m_wechatunionid;
    DCommandLinkButton *m_modButton;
};
} // namespace sync
} // namespace DCC_NAMESPACE
