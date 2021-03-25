/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "modifypasswdpage.h"
#include "widgets/titlelabel.h"
#include "pwqualitymanager.h"
#include "../../utils.h"
#include "createaccountpage.h"

#include "deepin_pw_check.h"

#include <DFontSizeManager>
#include <DDBusSender>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QDebug>

using namespace dcc::accounts;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::accounts;

ModifyPasswdPage::ModifyPasswdPage(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
{
    initWidget();
}

ModifyPasswdPage::~ModifyPasswdPage()
{
}

void ModifyPasswdPage::initWidget()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addSpacing(40);

    TitleLabel *titleLabel = new TitleLabel(tr("Change Password"));
    mainContentLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    mainContentLayout->addSpacing(40);

    QLabel *oldPasswdLabel = new QLabel(tr("Current Password") + ":");
    mainContentLayout->addWidget(oldPasswdLabel);
    mainContentLayout->addWidget(m_oldPasswordEdit);

    QLabel *newPasswdLabel = new QLabel(tr("New Password") + ":");
    mainContentLayout->addWidget(newPasswdLabel);
    mainContentLayout->addWidget(m_newPasswordEdit);

    QLabel *repeatPasswdLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addWidget(repeatPasswdLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);
    mainContentLayout->addStretch();

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->setSpacing(10);
    cansaveLayout->addWidget(cancleBtn);
    cansaveLayout->addWidget(saveBtn);
    mainContentLayout->addLayout(cansaveLayout);
    setLayout(mainContentLayout);
    cancleBtn->setDefault(true);
    saveBtn->setDefault(true);
    cancleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });

    connect(saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });

    connect(m_oldPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_oldPasswordEdit->isAlert()) {
            m_oldPasswordEdit->hideAlertMessage();
            m_oldPasswordEdit->setAlert(false);
        }
    });
    connect(m_newPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_newPasswordEdit->isAlert()) {
            m_newPasswordEdit->hideAlertMessage();
            m_newPasswordEdit->setAlert(false);
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_repeatPasswordEdit->isAlert()) {
            m_repeatPasswordEdit->hideAlertMessage();
            m_repeatPasswordEdit->setAlert(false);
        }
    });

    m_oldPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_oldPasswordEdit->setAccessibleName("oldpasswordedit");
    m_newPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_newPasswordEdit->setAccessibleName("newpasswordedit");
    m_repeatPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setAccessibleName("repeatpasswordedit");

    cancleBtn->setMinimumSize(165, 36);
    saveBtn->setMinimumSize(165, 36);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5);

    setFocusPolicy(Qt::StrongFocus);
}

bool ModifyPasswdPage::judgeTextEmpty(DPasswordEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"), edit, 2000);
    }

    return edit->text().isEmpty();
}

void ModifyPasswdPage::clickSaveBtn()
{
    //校验输入密码
    if (judgeTextEmpty(m_oldPasswordEdit) || judgeTextEmpty(m_newPasswordEdit) || judgeTextEmpty(m_repeatPasswordEdit)) return;

    Q_EMIT requestChangePassword(m_curUser, m_oldPasswordEdit->lineEdit()->text(), m_newPasswordEdit->lineEdit()->text(), m_repeatPasswordEdit->lineEdit()->text());
}

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode, const QString &errorTxt)
{
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_curUser->name(),
                                                                                      m_newPasswordEdit->lineEdit()->text());
    if (exitCode != 0) {
        if (errorTxt.startsWith("Current password: passwd:")) {
            m_oldPasswordEdit->setAlert(true);
            m_oldPasswordEdit->showAlertMessage(tr("Wrong password"));
            return;
        }

        if (m_newPasswordEdit->lineEdit()->text() == m_oldPasswordEdit->lineEdit()->text() ) {
            m_newPasswordEdit->setAlert(true);
            m_newPasswordEdit->showAlertMessage(tr("New password should differ from the current one"), m_oldPasswordEdit, 2000);
            return;
        }

        if (error == PW_NO_ERR) {
            if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
                m_repeatPasswordEdit->setAlert(true);
                m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
                return;
            }
        }

        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        // 企业版控制中心修改密码屏蔽安全中心登录安全的接口需求
        if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise) || (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC))
            return;

        // 密码校验失败并且安全中心密码安全等级不为低，弹出跳转到安全中心的对话框，低、中、高等级分别对应的值为1、2、3
        QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                                 QStringLiteral("/com/deepin/defender/daemonservice"),
                                 QStringLiteral("com.deepin.defender.daemonservice"));
        QDBusReply<int> level = interface.call("GetPwdLimitLevel");
        if (!interface.isValid()) {
            return;
        }
        if (level != 1) {
            QDBusReply<QString> errorTips = interface.call("GetPwdError");
            DDialog dlg("", errorTips, this);
            dlg.setIcon(QIcon::fromTheme("preferences-system"));
            dlg.addButton(tr("Go to Settings"));
            dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
            connect(&dlg, &DDialog::buttonClicked, this, [=](int idx) {
                if (idx == 0) {
                    DDBusSender()
                        .service("com.deepin.defender.hmiscreen")
                        .interface("com.deepin.defender.hmiscreen")
                        .path("/com/deepin/defender/hmiscreen")
                        .method(QString("ShowPage"))
                        .arg(QString("securitytools"))
                        .arg(QString("login-safety"))
                        .call();
                }
            });
            dlg.exec();
        }
    } else {
        Q_EMIT requestBack();
    }
}

//在修改密码页面当前密码处设置焦点
void ModifyPasswdPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if (m_oldPasswordEdit && !m_oldPasswordEdit->hasFocus()) {
        m_oldPasswordEdit->lineEdit()->setFocus();
    }
}
