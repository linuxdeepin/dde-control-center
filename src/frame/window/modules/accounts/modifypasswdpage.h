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

#pragma once

#include "window/namespace.h"
#include "modules/accounts/user.h"
#include "widgets/lineeditwidget.h"

#include <dpasswordedit.h>

#include <QWidget>

QT_BEGIN_NAMESPACE

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;

template <typename T>
class QList;

QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

//修改密码页面
class ModifyPasswdPage : public QWidget
{
    Q_OBJECT
public:
    explicit ModifyPasswdPage(dcc::accounts::User *user, QWidget *parent = nullptr);
    ~ModifyPasswdPage();
    void initWidget();
    void initData();
    void clickSaveBtn();
    void onPasswordChangeFinished(const int exitCode);
    bool validatePassword(const QString &password);
    bool containsChar(const QString &password, const QString &validate);
    void showErrorTip(QLineEdit *edit, const QString &error);
    void onDoEditFinish();

    enum ModifyPwdRetFlag {
        ModifyNewPwdSuccess = 0,
        InputOldPwdError  = 10
    };

private:
    void onEditFinished(Dtk::Widget::DPasswordEdit *t);

Q_SIGNALS:
    void requestChangePassword(dcc::accounts::User *userInter, const QString &oldPassword, const QString &password);
    void requestBack();

public Q_SLOTS:

private:
    dcc::accounts::User *m_curUser;
    QVBoxLayout *m_mainContentLayout;
    QHBoxLayout *m_cansaveLayout;
    QLabel *m_titleLabel;
    QLabel *m_oldPasswdLabel;
    QLabel *m_newPasswdLabel;
    QLabel *m_repeatPasswdLabel;
    QPushButton *m_cancleBtn;
    QPushButton *m_saveBtn;
    dcc::widgets::ErrorTip *m_errorTip;
    Dtk::Widget::DPasswordEdit *m_oldPasswordEdit;
    Dtk::Widget::DPasswordEdit *m_newPasswordEdit;
    Dtk::Widget::DPasswordEdit *m_repeatPasswordEdit;
    QList<Dtk::Widget::DPasswordEdit *> m_passeditList;
};

}
}
