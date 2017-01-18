#ifndef ACCOUNTSWORKER_H
#define ACCOUNTSWORKER_H

#include <QObject>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>
#include <org_freedesktop_displaymanager.h>
#include <org_freedesktop_displaymanager_session.h>

#include "user.h"
#include "usermodel.h"
#include "creationresult.h"

using com::deepin::daemon::Accounts;
using AccountsUser = com::deepin::daemon::accounts::User;
using CreationResult = dcc::accounts::CreationResult;

using org::freedesktop::DisplayManager;
using org::freedesktop::displaymanager::Session;

namespace dcc {
namespace accounts {

class AccountsWorker : public QObject
{
    Q_OBJECT

public:
    explicit AccountsWorker(UserModel * userList, QObject *parent = 0);

    void active();

signals:
    void requestFrameAutoHide(const bool autoHide) const;
    void accountCreationFinished(CreationResult *result) const;

public slots:
    void randomUserIcon(User *user);
    void createAccount(const User *user);

    void addNewAvatar(User *user);
    void setAvatar(User *user, const QString &iconPath);
    void deleteAccount(User *user, const bool deleteHome);
    void setAutoLogin(User *user, const bool autoLogin);
    void onUserListChanged(const QStringList &userList);
    void setPassword(User *user, const QString &passwd);
    void deleteUserIcon(User *user, const QString &iconPath);

    void addUser(const QString &userPath);
    void removeUser(const QString &userPath);

private slots:
    void updateUserOnlineStatus(const QList<QDBusObjectPath> paths);

private:
    AccountsUser *userInter(const QString &userName) const;
    CreationResult *createAccountInternal(const User *user);

private:
    Accounts *m_accountsInter;
    QSet<QString> m_userSet;
    QMap<User *, AccountsUser *> m_userInters;

    DisplayManager *m_dmInter;
    QStringList m_onlineUsers;

    UserModel *m_userModel;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSWORKER_H
