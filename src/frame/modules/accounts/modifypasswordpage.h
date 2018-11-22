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

#ifndef MODIFYPASSWORDPAGE_H
#define MODIFYPASSWORDPAGE_H

#include "widgets/contentwidget.h"
#include "user.h"
#include "widgets/lineeditwidget.h"

#include "widgets/buttontuple.h"

namespace dcc {
namespace accounts {

class ModifyPasswordPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ModifyPasswordPage(User *user, QWidget *parent = 0);

Q_SIGNALS:
    void requestChangePassword(User *userInter, const QString &oldPassword, const QString &password) const;

private Q_SLOTS:
    void passwordSubmit();
    void checkPwd();
    void onPasswordChangeFinished(const int exitCode);
    void updateTitle();
    void hideAlert();
    bool validatePassword(const QString &password);
    bool ContainsChar(const QString &password, const QString &validate);

private:
    template <typename T>
    void onEditFinished(T t);

private:
    User *m_userInter;

    dcc::widgets::LineEditWidget *m_oldpwdEdit;
    dcc::widgets::LineEditWidget *m_pwdEdit;
    dcc::widgets::LineEditWidget *m_pwdEditRepeat;
    dcc::widgets::ButtonTuple *m_buttonTuple;
};

} // namespace accounts
} // namespace dcc

#endif // MODIFYPASSWORDPAGE_H
