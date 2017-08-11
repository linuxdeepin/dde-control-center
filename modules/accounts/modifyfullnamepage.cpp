#include "modifyfullnamepage.h"
#include "user.h"
#include "settingsgroup.h"
#include "lineeditwidget.h"

#include <QPushButton>

using namespace dcc::widgets;

namespace dcc {
namespace accounts {

ModifyFullNamePage::ModifyFullNamePage(User *u, QWidget *parent)
    : ContentWidget(parent),

      m_user(u)
{
    m_fullnameWidget = new LineEditWidget;
    m_fullnameWidget->setTitle(tr("Fullname:"));
    m_fullnameWidget->textEdit()->setText(m_user->fullname());

    QPushButton *confirmBtn = new QPushButton;
    confirmBtn->setText(tr("OK"));

    SettingsGroup *grp = new SettingsGroup;
    grp->appendItem(m_fullnameWidget);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(grp);
    centralLayout->addWidget(confirmBtn);
    centralLayout->setSpacing(10);
    centralLayout->setContentsMargins(0, 10, 0, 0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Fullname Settings"));

    connect(m_user, &User::fullnameChanged, this, &ModifyFullNamePage::onFullnameChanged);
    connect(confirmBtn, &QPushButton::clicked, this, [=] { emit requestSetFullname(m_user, m_fullnameWidget->textEdit()->text()); });
}

void ModifyFullNamePage::onFullnameChanged(const QString &fn)
{
    m_fullnameWidget->textEdit()->setText(fn);
}

}

}
