/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
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

#include "securityquestionswidget.h"
#include "pwqualitymanager.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include <DFontSizeManager>

DGUI_USE_NAMESPACE

SecurityQuestionsWidget::SecurityQuestionsWidget(QList<int> questions, const QString &userName, QWidget *parent)
    : QWidget(parent)
    , m_userName(userName)
    , m_questionEdit1(new DLineEdit)
    , m_questionEdit2(new DLineEdit)
    , m_questionEdit3(new DLineEdit)
    , m_answerEdit1(new DLineEdit)
    , m_answerEdit2(new DLineEdit)
    , m_answerEdit3(new DLineEdit)
    , m_answerQuestionsWidget(new QWidget)
    , m_passwordWidget(new PasswordWidget(180, userName))
    , m_bAnswersRight(false)
    , m_stackedLayout(new QStackedLayout)
    , m_questions(questions)
{
    initWidget();
    initData();
}

bool SecurityQuestionsWidget::onResetPasswordBtnClicked()
{
    if (!m_bAnswersRight) {
        if (checkAnswers()) {
            m_stackedLayout->setCurrentIndex(1);
            Q_EMIT answersRight();
        }
        return false;
    }

    if (m_passwordWidget->isPasswordEmpty()) {
        return false;
    }
    if (!m_passwordWidget->checkPassword())
         return false;

    return true;
}

void SecurityQuestionsWidget::initWidget()
{
    this->setAccessibleName("SecurityQuestionsWidget");

    QVBoxLayout *answerQuestionsVLayout = new QVBoxLayout(m_answerQuestionsWidget);
    answerQuestionsVLayout->setSpacing(0);
    answerQuestionsVLayout->setMargin(0);

    m_questionEdit1->setClearButtonEnabled(false);
    m_questionEdit1->setFocusPolicy(Qt::NoFocus);
    m_questionEdit1->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_questionEdit2->setClearButtonEnabled(false);
    m_questionEdit2->setFocusPolicy(Qt::NoFocus);
    m_questionEdit2->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_questionEdit3->setClearButtonEnabled(false);
    m_questionEdit3->setFocusPolicy(Qt::NoFocus);
    m_questionEdit3->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    DFontSizeManager *fontManager =  DFontSizeManager::instance();
    fontManager->bind(m_questionEdit1, DFontSizeManager::T6, QFont::Medium);
    fontManager->bind(m_questionEdit2, DFontSizeManager::T6, QFont::Medium);
    fontManager->bind(m_questionEdit3, DFontSizeManager::T6, QFont::Medium);

    answerQuestionsVLayout->addWidget(m_questionEdit1);
    answerQuestionsVLayout->addSpacing(10);
    answerQuestionsVLayout->addWidget(m_answerEdit1);
    answerQuestionsVLayout->addSpacing(20);

    answerQuestionsVLayout->addWidget(m_questionEdit2);
    answerQuestionsVLayout->addSpacing(10);
    answerQuestionsVLayout->addWidget(m_answerEdit2);
    answerQuestionsVLayout->addSpacing(20);

    answerQuestionsVLayout->addWidget(m_questionEdit3);
    answerQuestionsVLayout->addSpacing(10);
    answerQuestionsVLayout->addWidget(m_answerEdit3);
    answerQuestionsVLayout->addSpacing(102);

    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setSpacing(0);
    mainContentLayout->setMargin(0);
    mainContentLayout->addSpacing(20);
    mainContentLayout->addLayout(m_stackedLayout);

    m_stackedLayout->setSpacing(0);
    m_stackedLayout->setMargin(0);
    m_stackedLayout->addWidget(m_answerQuestionsWidget);
    m_stackedLayout->addWidget(m_passwordWidget);

    mainContentLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_stackedLayout->setCurrentIndex(0);
}

void SecurityQuestionsWidget::initData()
{
    m_answerEdit1->setPlaceholderText(tr("Required"));
    m_answerEdit2->setPlaceholderText(tr("Required"));
    m_answerEdit3->setPlaceholderText(tr("Required"));
    QMap<int, QString> securityQuestions = {
        { 1, tr("What's the name of the city where you were born?")  },
        { 2, tr("What's the name of the first school you attended?") },
        { 3, tr("Who do you love the most in this world?")           },
        { 4, tr("What's your favorite animal?")                      },
        { 5, tr("What's your favorite song?")                        },
        { 6, tr("What's your nickname?")                             }};
    m_securityQuestions = securityQuestions;
    for (int i = 0; i < m_questions.size(); ++i) {
        if (i == 0) {
            m_questionEdit1->setText("1. " + m_securityQuestions.value(m_questions.at(i)));
        } else if (i == 1) {
            m_questionEdit2->setText("2. " + m_securityQuestions.value(m_questions.at(i)));
        } else if (i == 2) {
            m_questionEdit3->setText("3. " + m_securityQuestions.value(m_questions.at(i)));
        }
    }
    m_questionEdit1->lineEdit()->setCursorPosition(0);
    m_questionEdit2->lineEdit()->setCursorPosition(0);
    m_questionEdit3->lineEdit()->setCursorPosition(0);

    connect(this, &SecurityQuestionsWidget::requestSetPasswordHint, m_passwordWidget, &PasswordWidget::requestSetPasswordHint);
}

bool SecurityQuestionsWidget::checkAnswers()
{
    if (isAnswerEmpty())
        return false;

    QMap<int, QString> answers;
    for (int i = 0; i < m_questions.size(); ++i) {
        if (i == 0) {
            answers.insert(m_questions.at(i), m_answerEdit1->text());
            m_answerEdits.insert(m_questions.at(i), m_answerEdit1);
        } else if (i == 1) {
            answers.insert(m_questions.at(i), m_answerEdit2->text());
            m_answerEdits.insert(m_questions.at(i), m_answerEdit2);
        } else if (i == 2) {
            answers.insert(m_questions.at(i), m_answerEdit3->text());
            m_answerEdits.insert(m_questions.at(i), m_answerEdit3);
        }
    }
    Q_EMIT requestVerifySecretQuestions(answers);

    return m_bAnswersRight;
}

void SecurityQuestionsWidget::onVerifySecretQuestionsReplied(const QList<int> securityQuestions)
{
    for (auto question : securityQuestions) {
         auto answerEdit = m_answerEdits.value(question);
         if (answerEdit) {
             answerEdit->setAlert(true);
             answerEdit->showAlertMessage(tr("Wrong answer"), answerEdit, 2000);
         }
    }
    m_bAnswersRight = securityQuestions.isEmpty();
}

bool SecurityQuestionsWidget::isAnswerEmpty()
{
    return isContentEmpty(m_answerEdit1) || isContentEmpty(m_answerEdit2) || isContentEmpty(m_answerEdit3);
}
