// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SECURITY_LEVEL_ITEM_BINDER_H
#define SECURITY_LEVEL_ITEM_BINDER_H

#include "pwqualitymanager.h"
#include "widgets/securitylevelitem.h"

#include <DLineEdit>

#include <QApplication>

using namespace dcc::widgets;

namespace DCC_NAMESPACE {
namespace accounts {

class SecurityLevelItemBinder
{

public:
    static void bind(SecurityLevelItem *securityLevelItem, DLineEdit *lineEdit)
    {
        QObject::connect(lineEdit, &DLineEdit::textChanged, [securityLevelItem, lineEdit] (const QString &text) {
            if (text.isEmpty()) {
                securityLevelItem->setLevel(SecurityLevelItem::NoneLevel);
                lineEdit->setAlert(false);
                lineEdit->hideAlertMessage();
                return;
            }
            PwqualityManager *pwQualityManager = PwqualityManager::instance();
            PASSWORD_LEVEL_TYPE m_level = pwQualityManager->GetNewPassWdLevel(text);
            const QString &userName = qApp->property("editing_username").toString();
            PwqualityManager::ERROR_TYPE error = pwQualityManager->verifyPassword(userName, text);

            if (m_level == PASSWORD_STRENGTH_LEVEL_HIGH) {
                securityLevelItem->setLevel(SecurityLevelItem::HighLevel);
                if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
                    lineEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
                    lineEdit->setAlert(true);
                    lineEdit->showAlertMessage(pwQualityManager->getErrorTips(error), lineEdit, 2000);
                } else {
                    lineEdit->setAlert(false);
                    lineEdit->hideAlertMessage();
                }
            } else if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
                securityLevelItem->setLevel(SecurityLevelItem::MidLevel);
                if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
                    lineEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
                    lineEdit->setAlert(true);
                    lineEdit->showAlertMessage(pwQualityManager->getErrorTips(error), lineEdit, 2000);
                } else {
                    lineEdit->setAlert(false);
                    lineEdit->hideAlertMessage();
                }
            } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
                securityLevelItem->setLevel(SecurityLevelItem::LowLevel);
                if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
                    lineEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
                    lineEdit->setAlert(true);
                    lineEdit->showAlertMessage(pwQualityManager->getErrorTips(error), lineEdit, 2000);
                } else {
                    lineEdit->setAlert(false);
                    lineEdit->hideAlertMessage();
                }
            } else {
                lineEdit->showAlertMessage(QObject::tr("Error occurred when reading the configuration files of password rules!"));
            }
        });
    }
};

}
}

#endif //SECURITY_LEVEL_ITEM_BINDER_H
