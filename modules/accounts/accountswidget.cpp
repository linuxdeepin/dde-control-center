#include "accountswidget.h"

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc;

AccountsWidget::AccountsWidget()
    : ModuleWidget(),

    m_userGroup(new SettingsGroup)
{
    m_centeralLayout->addWidget(m_userGroup);

    m_nextPage = new QPushButton;
    m_nextPage->setText("Next Page >");
    m_centeralLayout->addWidget(m_nextPage);
    m_centeralLayout->addStretch(0);

    setTitle(tr("Accounts"));

    connect(m_nextPage, &QPushButton::clicked, this, &AccountsWidget::showAccountsDetail);
}

void AccountsWidget::addUser(User *user)
{
    NextPageWidget *w = new NextPageWidget;
    w->setTitle(user->name());

    m_userGroup->appendItem(w);

    connect(user, &User::nameChanged, w, &NextPageWidget::setTitle);
}

void AccountsWidget::removeUser(User *user)
{
    // TODO
}
