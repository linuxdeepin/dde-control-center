#ifndef ACCOUNTSWIDGET_H
#define ACCOUNTSWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
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

signals:
    void showAccountsDetail(User *account) const;
    void requestCreateAccount() const;

public slots:
    void addUser(User *user);
    void removeUser(User *user);

private:
    dcc::widgets::SettingsGroup *m_userGroup;

    QPushButton *m_createBtn;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSWIDGET_H
