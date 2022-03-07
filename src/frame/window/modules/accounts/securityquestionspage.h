/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
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

#include "accountswidget.h"

#include <DDialog>
#include <DLineEdit>
#include <DComboBox>

#define SECURITY_QUESTIONS_NUM 6
#define SECURITY_ANSWERS_CHARACTERS_MAX_SIZE 8

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

class SecurityQuestionsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityQuestionsPage(dcc::accounts::User *user, QWidget *parent = nullptr);
    ~SecurityQuestionsPage();

Q_SIGNALS:
    void requestBack(DCC_NAMESPACE::accounts::AccountsWidget::ActionOption option = DCC_NAMESPACE::accounts::AccountsWidget::ClickCancel);
    void requestSetSecurityQuestions(dcc::accounts::User *userInter, const QMap<int, QByteArray> &securityQuestions);

public Q_SLOTS:
    void onConfirmButtonClicked();
    void onQuestionCombobox1CurrentTextChanged(int);
    void onQuestionCombobox2CurrentTextChanged(int);
    void onQuestionCombobox3CurrentTextChanged(int);
    void onAnswerEdit1CurrentTextChanged(const QString&);
    void onAnswerEdit2CurrentTextChanged(const QString&);
    void onAnswerEdit3CurrentTextChanged(const QString&);
    void onSetSecurityQuestionsReplied(const QString &errorText);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void initWidget();
    void initData();
    void addItems(DComboBox *questionCombobox);
    bool isContentEmpty(DComboBox *edit);
    bool isContentEmpty(DLineEdit *edit);
    bool isSecurityQuestionsEmpty();
    QString cryptUserPassword(const QString &password);
    bool isAllAnswersCharactersSizeRight();
    bool isAnswersCharactersSizeRight(DLineEdit *edit);
    void checkQuestionDuplicate(int id, int id1, int id2, QWidget* w);

private:
    dcc::accounts::User *m_curUser;
    DComboBox *m_questionCombobox1;
    DComboBox *m_questionCombobox2;
    DComboBox *m_questionCombobox3;
    DLineEdit *m_answerEdit1;
    DLineEdit *m_answerEdit2;
    DLineEdit *m_answerEdit3;
};

}
}

