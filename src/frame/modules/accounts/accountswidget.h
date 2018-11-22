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

#ifndef ACCOUNTSWIDGET_H
#define ACCOUNTSWIDGET_H

#include "modules/modulewidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "usermodel.h"
#include "user.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc {
namespace accounts {

class AccountsWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit AccountsWidget();

#ifdef DCC_ENABLE_ADDOMAIN
    void setModel(UserModel * const model);
#endif

Q_SIGNALS:
    void showAccountsDetail(User *account) const;
    void requestCreateAccount() const;
#ifdef DCC_ENABLE_ADDOMAIN
    void requestShowADDialog() const;
    void requestRefreshADState() const;
#endif

public Q_SLOTS:
    void addUser(User *user);
#ifdef DCC_ENABLE_ADDOMAIN
    void setADState(bool isJoin);

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
#endif

private:
    dcc::widgets::SettingsGroup *m_userGroup;
    QPushButton *m_createBtn;
#ifdef DCC_ENABLE_ADDOMAIN
    QPushButton *m_adBtn;
    UserModel *m_model;
#endif
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSWIDGET_H
