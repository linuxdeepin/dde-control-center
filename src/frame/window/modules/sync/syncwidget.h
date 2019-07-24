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

#include "window/namespace.h"

#include <QWidget>

class QStackedLayout;

namespace dcc {
namespace cloudsync {
class SyncModel;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class LoginPage;
class IndexPage;
class SyncWidget : public QWidget
{
    Q_OBJECT
public:
    SyncWidget(QWidget *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel* model);

Q_SIGNALS:
    void requestLoginUser() const;

private:
    void onUserInfoChanged(const QVariantMap &userInfo);

private:
    QStackedLayout *m_mainLayout;
    dcc::cloudsync::SyncModel* m_model;
    LoginPage* m_loginPage;
    IndexPage* m_indexPage;
};
} // namespace sync

} // namespace DCC_NAMESPACE
