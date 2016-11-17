#include "modifypasswordpage.h"
#include "settingsgroup.h"

#include <QVBoxLayout>

using namespace dcc;

ModifyPasswordPage::ModifyPasswordPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_pwdEdit(new LineEditWidget),
      m_pwdEditRepeat(new LineEditWidget),

      m_cancel(new QPushButton),
      m_accept(new QPushButton)
{
    m_pwdEdit->setTitle(tr("New Password"));
    m_pwdEditRepeat->setTitle(tr("Repeat Password"));

    m_cancel->setText(tr("Cancel"));
    m_accept->setText(tr("Accept"));

    SettingsGroup *pwdGroup = new SettingsGroup;
    pwdGroup->appendItem(m_pwdEdit);
    pwdGroup->appendItem(m_pwdEditRepeat);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_cancel);
    buttonsLayout->addWidget(m_accept);
    buttonsLayout->setSpacing(0);
    buttonsLayout->setMargin(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pwdGroup);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("Password") + " - " + user->name());
}
