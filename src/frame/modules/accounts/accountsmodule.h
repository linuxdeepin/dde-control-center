/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
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

#ifndef ACCOUNTSMODULE_H
#define ACCOUNTSMODULE_H

#include "interfaces/moduleinterface.h"
#include "accountswidget.h"
#include "accountsworker.h"
#include "user.h"
#include "usermodel.h"
#include "fingermodel.h"
#include "fingerworker.h"

#include <com_deepin_daemon_accounts.h>

namespace dcc {
namespace accounts {

class AccountsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit AccountsModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void showPage(const QString &pageName);

    ModuleWidget *moduleWidget();

    const QString name() const;

private:
    ~AccountsModule();

    void showAvatarPage(User *account);
    void showFullnamePage(User *account);
    void showPasswordPage(User *account);
    void showAccountsDetail(User *account);
    void showCreateAccountPage();
    void showFingerPage(User *account);
    void showAddThumb(const QString &name, const QString &thumb);
    void contentPopped(ContentWidget * const w);

#ifdef DCC_ENABLE_ADDOMAIN
    void showADDialog();
#endif

private Q_SLOTS:
    void setFrameAutoHide(const bool autoHide);

private:
    UserModel *m_userList;

    AccountsWidget *m_accountsWidget;
    AccountsWorker *m_accountsWorker;

    FingerWorker *m_fingerWorker;
    FingerModel *m_fingerModel;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSMODULE_H
