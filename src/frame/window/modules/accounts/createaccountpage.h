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
#include "modules/accounts/creationresult.h"
#include "avatarlistwidget.h"
#include "widgets/lineeditwidget.h"

#include <dpasswordedit.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

//创建账户页面
class CreateAccountPage : public QWidget
{
    Q_OBJECT
public:
    explicit CreateAccountPage(QWidget *parent = nullptr);
    ~CreateAccountPage();
    void setModel(dcc::accounts::User *user);

private:
    void initWidgets();
    void initDatas();
    void createUser();
    bool onPasswordEditFinished(DPasswordEdit *edit);
    bool validatePassword(const QString &password);
    bool containsChar(const QString &password, const QString &validate);
    void showErrorTip(QLineEdit *edit, const QString &error);
    bool validateUsername(const QString &username);
    bool onNameEditFinished();

Q_SIGNALS:
    void requestCreateUser(const dcc::accounts::User *user);
    void requestBack();

public Q_SLOTS:
    void setCreationResult(dcc::accounts::CreationResult *result);

private:
    dcc::accounts::User *m_newUser;
    QVBoxLayout *m_mainContentLayout;
    QHBoxLayout *m_titleLayout;
    QVBoxLayout *m_inputLayout;
    QHBoxLayout *m_selectLayout;
    QLabel *m_title;
    DCC_NAMESPACE::accounts::AvatarListWidget *m_avatarListWidget;
    QLabel *m_nameLabel;
    QLabel *m_fullnameLabel;
    QLabel *m_passwdLabel;
    QLabel *m_repeatpasswdLabel;
    QLineEdit *m_nameEdit;
    QLineEdit *m_fullnameEdit;
    Dtk::Widget::DPasswordEdit *m_passwdEdit;
    Dtk::Widget::DPasswordEdit *m_repeatpasswdEdit;
    QPushButton *m_cancleBtn;
    QPushButton *m_addBtn;
    dcc::widgets::ErrorTip *m_errorTip;
    QLineEdit *m_errorEdit;
};

}
}
