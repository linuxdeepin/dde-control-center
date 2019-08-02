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

#include <dimagebutton.h>

#include <QWidget>

QT_BEGIN_NAMESPACE

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QLineEdit;

template <class Key, class T>
class QMap;

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
    void initWidgets();
    void initDatas();
    void setModel(dcc::accounts::User *user);
    void createUser();
    template <typename T>
    void onEditFinished(T t);
    bool validatePassword(const QString &password);
    bool containsChar(const QString &password, const QString &validate);

    void showUsernameErrorTip(const QString &error);
    void showUserfullnameErrorTip(const QString &error);
    void showPasswordEmptyErrorTip(const QString &error);
    void showPasswordMatchErrorTip(const QString &error);

    void updateLineEditDisplayStyle();

Q_SIGNALS:
    void requestCreateUser(const dcc::accounts::User *user);
    void requestBack();

public Q_SLOTS:
    void setCreationResult(dcc::accounts::CreationResult *result);

private:
    dcc::accounts::User *m_newUser;
    QVBoxLayout *m_mainContentLayout;
    QVBoxLayout *m_inputLayout;
    QHBoxLayout *m_selectLayout;
    QVBoxLayout *m_nameLayout;
    QVBoxLayout *m_fullnameLayout;
    QVBoxLayout *m_passwdLayout;
    QVBoxLayout *m_repeatpasswdLayout;
    QHBoxLayout *m_passwdbtnLayout;
    QHBoxLayout *m_repeatpasswdbtnLayout;
    DCC_NAMESPACE::accounts::AvatarListWidget *m_avatarListWidget;
    QLabel *m_nameLabel;
    QLabel *m_fullnameLabel;
    QLabel *m_passwdLabel;
    QLabel *m_repeatpasswdLabel;
    QLineEdit *m_nameEdit;
    QLineEdit *m_fullnameEdit;
    QLineEdit *m_passwdEdit;
    QLineEdit *m_repeatpasswdEdit;
    QPushButton *m_cancleBtn;
    QPushButton *m_addBtn;
    dcc::widgets::ErrorTip *m_errorTip;
    Dtk::Widget::DImageButton *m_passwdBtn;
    Dtk::Widget::DImageButton *m_repeatpasswdBtn;
    QMap<Dtk::Widget::DImageButton*, QLineEdit*> m_editButtonMap;
};

}
}
