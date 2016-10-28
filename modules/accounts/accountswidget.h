#ifndef ACCOUNTSWIDGET_H
#define ACCOUNTSWIDGET_H

#include "modulewidget.h"
#include "contentwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "user.h"

#include <QPushButton>
#include <QVBoxLayout>

class AccountsWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit AccountsWidget();

signals:
    void showAccountsDetail() const;

public slots:
    void addUser(User *user);
    void removeUser(User *user);

private:
    dcc::SettingsGroup *m_userGroup;
    QPushButton *m_nextPage;
};

#endif // ACCOUNTSWIDGET_H
