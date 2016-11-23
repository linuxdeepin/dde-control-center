#include "accountswidget.h"

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingsheaderitem.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;

AccountsWidget::AccountsWidget()
    : ModuleWidget(),

    m_userGroup(new SettingsGroup)
{
    m_userGroup->setHeaderVisible(true);
    m_userGroup->headerItem()->setTitle(tr("Accounts"));
    m_centeralLayout->addWidget(m_userGroup);

    setTitle(tr("Accounts"));
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
    Q_UNUSED(user);
}
