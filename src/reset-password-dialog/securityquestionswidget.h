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

#ifndef SECURITYQUESTIONSWIDGET_H
#define SECURITYQUESTIONSWIDGET_H

#include <QStackedLayout>

#include "passwordwidget.h"

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
