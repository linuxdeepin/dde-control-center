/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "utils.h"
#include <dtkwidget_global.h>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DLabel;
class DLineEdit;
class DToolButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QObject;
class QWidget;
class QStandardItemModel;
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

class SyncModel;
class DownloadUrl;
class AvatarWidget;
class LoginInfoPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginInfoPage(QWidget *parent = nullptr);
    ~LoginInfoPage();

    void setModel(SyncModel *model);

Q_SIGNALS:
    void requestLogoutUser() const;
    void requestSetFullname(const QString &fullname);
    void requestPullMessage() const;

protected:
    void initUI();
    void initConnection();
    void onUserInfoChanged(const QVariantMap &infos);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void onEditingFinished(const QString& userFullName);
    void onAvatarChanged(const QString &avaPath);
    void onUserInfoListChanged(const QList<QPair<QString, QString>> &moduleTs);
    void setAvatarPath(const QString &avatarPath);
    void onUserUnbindInfoChanged(const QString& ubid);
    void resizeEvent(QResizeEvent *event);

private:
    QString handleNameTooLong(const QString &fullName, int base = 0);
    void onResetError(const QString &error);
    QString loadCodeURL();

private:
    QVBoxLayout *m_mainLayout;
    SyncModel *m_model;

    DownloadUrl *m_downloader;
    QString m_avatarPath;
    AvatarWidget *m_avatar;
    bool m_fristLogin; // 首次登录
    QString m_userFullName;

    DTK_WIDGET_NAMESPACE::DLabel *m_username;
    DTK_WIDGET_NAMESPACE::DToolButton *m_editNameBtn;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_inputLineEdit;

    // Info ListView
    DTK_WIDGET_NAMESPACE::DListView *m_listView;
    QStandardItemModel *m_listModel;

    QPushButton *m_logoutBtn;
    QPushButton *m_editInfoBtn;
};
