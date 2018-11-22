/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef DCC_ACCOUNTS_CREATEPAGE_H
#define DCC_ACCOUNTS_CREATEPAGE_H

#include <darrowrectangle.h>

#include "widgets/contentwidget.h"
#include "avatarwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/buttontuple.h"

#include "user.h"
#include "creationresult.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {

class CreatePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit CreatePage(QWidget *parent = 0);
    ~CreatePage();

    void setModel(User *user);

Q_SIGNALS:
    void requestCreateUser(const User *user) const;

public Q_SLOTS:
    void setCreationResult(CreationResult *result);

private:
    void createUser();
    void cancelCreation() const;

    void showUsernameErrorTip(QString error);
    void showPasswordEmptyErrorTip(const QString &error);
    void showPasswordMatchErrorTip(QString error);

    template <typename T>
    void onEditFinished(T t);
    bool validatePassword(const QString &password);
    bool ContainsChar(const QString &password, const QString &validate);

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    AvatarWidget *m_avatar;
    dcc::widgets::SettingsGroup *m_group;
    dcc::widgets::LineEditWidget *m_username;
    dcc::widgets::LineEditWidget *m_password;
    dcc::widgets::LineEditWidget *m_repeatpass;
    dcc::widgets::ButtonTuple *m_buttonTuple;

    widgets::ErrorTip *m_errorTip;

    User *m_user;
};

} // namespace accounts
} // namespace dcc

#endif // DCC_ACCOUNTS_CREATEPAGE_H
