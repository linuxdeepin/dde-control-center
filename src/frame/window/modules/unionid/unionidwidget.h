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
#include "modules/sync/utils.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLabel;
QT_END_NAMESPACE

using namespace dcc::cloudsync;

namespace dcc {
namespace unionid {
class UnionidModel;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace unionid {
class LoginPage;
class IndexPage;
class LogoutPage;
class UnionidWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UnionidWidget(QWidget *parent = nullptr);
    ~UnionidWidget();
    void setModel(dcc::unionid::UnionidModel *model, MainWindow *pMainWindow);
    void switchWidget(const QVariantMap &userInfo);
    void setDefault();

public Q_SLOTS:
    void onGetATFinished(bool bIsFinished);

Q_SIGNALS:
    void requestSignInUser() const;
    void requestLoginUser() const;
    void requestLogoutUser() const;
    void requestSetAutoSync(bool enable) const;
    void requestPopupDialog(QString) const;
    void requesUserDialog(QString) const;
    void requestSetModuleState(std::pair<SyncType, bool> state) const;

private:
    QStackedLayout *m_pageLayout;
    dcc::unionid::UnionidModel *m_model;
    LoginPage *m_loginPage;
    IndexPage *m_indexPage;
    LogoutPage *m_cnonlyPage;
};
} // namespace sync

} // namespace DCC_NAMESPACE
