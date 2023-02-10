//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "src/plugin-accounts/operation/user.h"
#include "src/plugin-accounts/operation/usermodel.h"

#include <DDialog>
#include <DLineEdit>
#include <DComboBox>

#define SECURITY_QUESTIONS_NUM 6
#define SECURITY_ANSWERS_CHARACTERS_MAX_SIZE 30

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class SecurityQuestionsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityQuestionsPage(User *user, QWidget *parent = nullptr);
    ~SecurityQuestionsPage();

Q_SIGNALS:
    void requestBack(UserModel::ActionOption option = UserModel::ClickCancel);
    void requestSetSecurityQuestions(User *userInter, const QMap<int, QByteArray> &securityQuestions);
    void requestSecurityQuestionsCheck(User *userInter);

public Q_SLOTS:
    void onConfirmButtonClicked();
    void onQuestionCombobox1CurrentTextChanged(int);
    void onQuestionCombobox2CurrentTextChanged(int);
    void onQuestionCombobox3CurrentTextChanged(int);
    void onAnswerEdit1CurrentTextChanged(const QString&);
    void onAnswerEdit2CurrentTextChanged(const QString&);
    void onAnswerEdit3CurrentTextChanged(const QString&);
    void onSecurityQuestionsCheckReplied(const QList<int> &questions);
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
    void hideAlert(DLineEdit *edit);

private:
    User *m_curUser;
    DComboBox *m_questionCombobox1;
    DComboBox *m_questionCombobox2;
    DComboBox *m_questionCombobox3;
    DLineEdit *m_answerEdit1;
    DLineEdit *m_answerEdit2;
    DLineEdit *m_answerEdit3;
};

}

