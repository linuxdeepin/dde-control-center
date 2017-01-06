#include "modifypasswordpage.h"
#include "settingsgroup.h"
#include "translucentframe.h"

#include <QVBoxLayout>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::accounts;

ModifyPasswordPage::ModifyPasswordPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_userInter(user),

      m_pwdEdit(new LineEditWidget),
      m_pwdEditRepeat(new LineEditWidget),

      m_cancel(new QPushButton),
      m_accept(new QPushButton)
{
    m_pwdEdit->textEdit()->setEchoMode(QLineEdit::Password);
    m_pwdEdit->setTitle(tr("New Password"));
    m_pwdEditRepeat->textEdit()->setEchoMode(QLineEdit::Password);
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

    TranslucentFrame *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("Password") + " - " + user->name());

    connect(m_accept, &QPushButton::clicked, this, &ModifyPasswordPage::passwordSubmit);
    connect(m_cancel, &QPushButton::clicked, this, &ModifyPasswordPage::back);
    connect(m_pwdEdit->textEdit(), &QLineEdit::editingFinished, this, &ModifyPasswordPage::checkPwd);
    connect(m_pwdEditRepeat->textEdit(), &QLineEdit::editingFinished, this, &ModifyPasswordPage::checkPwd);
}

void ModifyPasswordPage::passwordSubmit()
{
    const QString pwd0 = m_pwdEdit->textEdit()->text();
    const QString pwd1 = m_pwdEditRepeat->textEdit()->text();

    if (pwd0 != pwd1)
        return;

    emit requestChangePassword(m_userInter, pwd0);
    emit back();
}

void ModifyPasswordPage::checkPwd()
{
    m_pwdEdit->setIsErr(m_pwdEdit->text().isEmpty());
    m_pwdEditRepeat->setIsErr(m_pwdEditRepeat->text() != m_pwdEdit->text());
}
