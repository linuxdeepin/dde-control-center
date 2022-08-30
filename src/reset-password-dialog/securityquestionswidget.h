// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SECURITYQUESTIONSWIDGET_H
#define SECURITYQUESTIONSWIDGET_H

#include <QStackedLayout>

#include "passwordwidget.h"

#define SECURITY_ANSWERS_CHARACTERS_MAX_SIZE 30

class SecurityQuestionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SecurityQuestionsWidget(QList<int>questions, const QString &userName, QWidget *parent = nullptr);
    ~SecurityQuestionsWidget() {}
    bool onResetPasswordBtnClicked();
    inline const QString getPassword() { return m_passwordWidget->getPassword(); }
    bool isAnswerRight() { return m_bAnswersRight; }

private:
    void initWidget();
    void initData();
    bool checkAnswers();
    bool isAnswerEmpty();
    bool isAnswerSizeRight(DLineEdit *edit);
    bool isAllAnswersSizeRight();

Q_SIGNALS:
    void answersRight();
    void requestSetPasswordHint(const QString &passwordHint);
    void requestVerifySecretQuestions(QMap<int, QString> &secretQuestions);

public Q_SLOTS:
    void onVerifySecretQuestionsReplied(const QList<int> securityQuestions);

private:
    QString m_userName;
    DLineEdit *m_questionEdit1;
    DLineEdit *m_questionEdit2;
    DLineEdit *m_questionEdit3;
    DLineEdit *m_answerEdit1;
    DLineEdit *m_answerEdit2;
    DLineEdit *m_answerEdit3;
    QWidget *m_answerQuestionsWidget;
    PasswordWidget *m_passwordWidget;
    bool m_bAnswersRight;
    QStackedLayout *m_stackedLayout;
    QMap<int, QString> m_securityQuestions;
    QList<int> m_questions;
    QMap<int, DLineEdit*> m_answerEdits;
};

#endif // REMINDERDDIALOG_H
