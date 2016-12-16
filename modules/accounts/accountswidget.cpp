#include "accountswidget.h"

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingsheaderitem.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::accounts;

AccountsWidget::AccountsWidget()
    : ModuleWidget(),

    m_userGroup(new dcc::widgets::SettingsGroup),
    m_createBtn(new QPushButton(tr("Create Account")))
{
    setObjectName("Accounts");

    m_centeralLayout->addWidget(m_userGroup);
    m_centeralLayout->addWidget(m_createBtn);

    setTitle(tr("Accounts"));

    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

void AccountsWidget::addUser(User *user)
{
    NextPageWidget *w = new NextPageWidget;
    w->setTitle(user->name());

    m_userGroup->appendItem(w);

    connect(user, &User::nameChanged, w, &NextPageWidget::setTitle);
    connect(w, &NextPageWidget::clicked, [=] { emit showAccountsDetail(user); });
}

void AccountsWidget::removeUser(User *user)
{
    QList<NextPageWidget *> items = findChildren<NextPageWidget*>();
    for (NextPageWidget *item : items) {
        if (item->title() == user->name()) {
            m_userGroup->removeItem(item);
            item->deleteLater();
            break;
        }
    }
}
