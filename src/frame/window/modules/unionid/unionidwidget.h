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
#include "modules/modulewidget.h"
#include <QWidget>
#include <DWarningButton>
QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace unionid {
class UnionidModel;
}
}



namespace dcc {
namespace cloudsync {
enum SyncType : int;
enum UniondidState : int;
}
}

namespace DCC_NAMESPACE {
namespace unionid {
class LoginPage;
class IndexPage;
class LogoutPage;
class UnionidWidget : public QWidget
{
    Q_OBJECT
public:
    UnionidWidget(QWidget *parent = nullptr);
    void setModel(dcc::unionid::UnionidModel *model);

Q_SIGNALS:
    void requestLoginUser() const;
    void requestLogoutUser() const;
    void requestSetAutoSync(bool enable) const;
    void requestPopupDialog(QString) const;
    void requesUserDialog(QString) const;
private:
    void onUserInfoChanged(const QVariantMap &userInfo);

private:
    QStackedLayout *m_pageLayout;
    dcc::unionid::UnionidModel *m_model;
    LoginPage *m_loginPage;
    IndexPage *m_indexPage;
    LogoutPage *m_cnonlyPage;
    DTK_NAMESPACE::Widget::DWarningButton *m_logoutBtn{nullptr};
    QLabel *m_label;
};
} // namespace sync

} // namespace DCC_NAMESPACE
