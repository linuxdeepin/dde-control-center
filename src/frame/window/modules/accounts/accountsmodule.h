/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "window/interface/moduleinterface.h"
#include "accountswidget.h"
#include "modules/accounts/accountsworker.h"
#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"
#include "modules/accounts/fingermodel.h"
#include "modules/accounts/fingerworker.h"
#include "window/namespace.h"

#include <com_deepin_daemon_accounts.h>

namespace DCC_NAMESPACE {
namespace accounts {

class AccountsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit AccountsModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize();
    void reset();
    const QString name() const;
    void showPage(const QString &pageName);
    QWidget *moduleWidget();
    void contentPopped(QWidget *const w);

private:
    ~AccountsModule();

private:
    UserModel *m_userList;
    AccountsWidget *m_accountsWidget;
    AccountsWorker *m_accountsWorker;
};

}   // namespace accounts
}   // namespace dccV20
