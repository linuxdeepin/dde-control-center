#ifndef ACCOUNTSMODULE_H
#define ACCOUNTSMODULE_H

#include "moduleinterface.h"
#include "accountswidget.h"
#include "accountsworker.h"
#include "user.h"
#include "usermodel.h"

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
    void contentPopped(ContentWidget * const w);

private slots:
    void setFrameAutoHide(const bool autoHide);

private:
    UserModel *m_userList;

    AccountsWidget *m_accountsWidget;
    AccountsWorker *m_accountsWorker;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSMODULE_H
