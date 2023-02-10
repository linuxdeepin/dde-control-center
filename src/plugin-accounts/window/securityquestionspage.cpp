//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "securityquestionspage.h"
#include "createaccountpage.h"

#include <DMessageManager>
#include <DAlertControl>
#include <DGuiApplicationHelper>

#include <QDebug>

#include <unistd.h>
#include <random>


using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

static void palrtteTransparency(QWidget *widget, qint8 alphaFloat)
{
    QPalette palette = widget->palette();
    QColor color = DGuiApplicationHelper::adjustColor(palette.color(QPalette::Active, QPalette::BrightText), 0, 0, 0, 0, 0, 0, alphaFloat);
    palette.setColor(QPalette::WindowText, color);
    widget->setPalette(palette);
}

SecurityQuestionsPage::SecurityQuestionsPage(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_questionCombobox1(new DComboBox)
    , m_questionCombobox2(new DComboBox)
    , m_questionCombobox3(new DComboBox)
    , m_answerEdit1(new DLineEdit)
    , m_answerEdit2(new DLineEdit)
    , m_answerEdit3(new DLineEdit)
{
    initWidget();
    initData();
}

SecurityQuestionsPage::~SecurityQuestionsPage()
{

}

void SecurityQuestionsPage::initWidget()
{
    this->setAccessibleName("SecurityQuestionsPage");

    QLabel *titleLabel = new QLabel(tr("Security Questions"));
    titleLabel->setObjectName("TitleLabel");
    titleLabel->setAccessibleName("DDialogTitleLabel");
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    titleLabel->setWordWrap(true);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    DFontSizeManager *fontManager =  DFontSizeManager::instance();
    fontManager->bind(titleLabel, DFontSizeManager::T5, QFont::Medium);
    palrtteTransparency(titleLabel, -10);

    QLabel *messageLabel = new QLabel(tr("These questions will be used to help reset your password in case you forget it."));
    fontManager->bind(messageLabel, DFontSizeManager::T6, QFont::Medium);
    messageLabel->setObjectName("MessageLabel");
    messageLabel->setAccessibleName("DDialogMessageLabel");
    messageLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    palrtteTransparency(messageLabel, -30);

    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setContentsMargins(10, 20, 10, 10);
    mainContentLayout->setSpacing(0);

    mainContentLayout->addWidget(titleLabel);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(messageLabel);
    mainContentLayout->addSpacing(20);

    QLabel *questionLabel1 = new QLabel(tr("Security question 1") + ":");
    mainContentLayout->addWidget(questionLabel1, 0, Qt::AlignLeft);
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(m_questionCombobox1);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(m_answerEdit1);
    mainContentLayout->addSpacing(20);

    QLabel *questionLabel2 = new QLabel(tr("Security question 2") + ":");
    mainContentLayout->addWidget(questionLabel2, 0, Qt::AlignLeft);
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(m_questionCombobox2);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(m_answerEdit2);
    mainContentLayout->addSpacing(20);

    QLabel *questionLabel3 = new QLabel(tr("Security question 3") + ":");
    mainContentLayout->addWidget(questionLabel3, 0, Qt::AlignLeft);
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(m_questionCombobox3);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(m_answerEdit3);
    mainContentLayout->addStretch(0);

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    DSuggestButton *confirmButton = new DSuggestButton(tr("Confirm"));
    QHBoxLayout *buttonHLayout = new QHBoxLayout;
    buttonHLayout->setSpacing(0);
    buttonHLayout->addWidget(cancelButton);
    buttonHLayout->addSpacing(10);
    buttonHLayout->addWidget(confirmButton);

    mainContentLayout->addStretch();
    mainContentLayout->addLayout(buttonHLayout);

    addItems(m_questionCombobox1);
    addItems(m_questionCombobox2);
    addItems(m_questionCombobox3);

    connect(cancelButton, &QPushButton::clicked, this, [this] {
        // 关闭当前界面时需要断开信号连接
        disconnect(m_curUser, &User::startSecurityQuestionsCheckReplied, this, &SecurityQuestionsPage::onSecurityQuestionsCheckReplied);
        disconnect(m_curUser, &User::setSecurityQuestionsReplied, this, &SecurityQuestionsPage::onSetSecurityQuestionsReplied);
        Q_EMIT requestBack();
    });

    connect(confirmButton, &QPushButton::clicked, this, &SecurityQuestionsPage::onConfirmButtonClicked);
    connect(m_questionCombobox1, qOverload<int>(&QComboBox::currentIndexChanged), this, &SecurityQuestionsPage::onQuestionCombobox1CurrentTextChanged);
    connect(m_questionCombobox2, qOverload<int>(&QComboBox::currentIndexChanged), this, &SecurityQuestionsPage::onQuestionCombobox2CurrentTextChanged);
    connect(m_questionCombobox3, qOverload<int>(&QComboBox::currentIndexChanged), this, &SecurityQuestionsPage::onQuestionCombobox3CurrentTextChanged);
    connect(m_answerEdit1, &DLineEdit::textChanged, this, &SecurityQuestionsPage::onAnswerEdit1CurrentTextChanged);
    connect(m_answerEdit2, &DLineEdit::textChanged, this, &SecurityQuestionsPage::onAnswerEdit2CurrentTextChanged);
    connect(m_answerEdit3, &DLineEdit::textChanged, this, &SecurityQuestionsPage::onAnswerEdit3CurrentTextChanged);
    connect(m_curUser, &User::startSecurityQuestionsCheckReplied, this, &SecurityQuestionsPage::onSecurityQuestionsCheckReplied);
    connect(m_curUser, &User::setSecurityQuestionsReplied, this, &SecurityQuestionsPage::onSetSecurityQuestionsReplied);

    m_answerEdit1->setFocus();
}

void SecurityQuestionsPage::initData()
{
    m_answerEdit1->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    m_answerEdit2->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    m_answerEdit3->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    m_answerEdit1->setPlaceholderText(tr("Keep the answer under 30 characters"));
    m_answerEdit2->setPlaceholderText(tr("Keep the answer under 30 characters"));
    m_answerEdit3->setPlaceholderText(tr("Keep the answer under 30 characters"));
}

void SecurityQuestionsPage::onConfirmButtonClicked()
{
    if (isSecurityQuestionsEmpty()) {
        return;
    }

    int index1 = m_questionCombobox1->currentIndex();
    int index2 = m_questionCombobox2->currentIndex();
    int index3 = m_questionCombobox3->currentIndex();

    if ((index1 == index2) || (index2 == index3 ) || (index1 == index3)) {
        DMessageManager::instance()->sendMessage(this,
                                                 style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 tr("Do not choose a duplicate question please"));
        return;
    }

    if (!isAllAnswersCharactersSizeRight()) {
        return;
    }

    QMap<int, QByteArray> securityQuestions {
        {index1, cryptUserPassword(m_answerEdit1->text()).toUtf8()},
        {index2, cryptUserPassword(m_answerEdit2->text()).toUtf8()},
        {index3, cryptUserPassword(m_answerEdit3->text()).toUtf8()}};

    Q_EMIT requestSetSecurityQuestions(m_curUser, securityQuestions);
}

void SecurityQuestionsPage::onQuestionCombobox1CurrentTextChanged(int index)
{
    m_answerEdit1->clear();
    checkQuestionDuplicate(index, m_questionCombobox2->currentIndex(), m_questionCombobox3->currentIndex(), m_questionCombobox1);
}

void SecurityQuestionsPage::onQuestionCombobox2CurrentTextChanged(int index)
{
    m_answerEdit2->clear();
    checkQuestionDuplicate(index, m_questionCombobox1->currentIndex(), m_questionCombobox3->currentIndex(), m_questionCombobox2);
}

void SecurityQuestionsPage::onQuestionCombobox3CurrentTextChanged(int index)
{
    m_answerEdit3->clear();
    checkQuestionDuplicate(index, m_questionCombobox1->currentIndex(), m_questionCombobox2->currentIndex(), m_questionCombobox3);
}

void SecurityQuestionsPage::onAnswerEdit1CurrentTextChanged(const QString &)
{
    hideAlert(m_answerEdit1);
}

void SecurityQuestionsPage::onAnswerEdit2CurrentTextChanged(const QString &)
{
    hideAlert(m_answerEdit2);
}

void SecurityQuestionsPage::onAnswerEdit3CurrentTextChanged(const QString &)
{
    hideAlert(m_answerEdit3);
}

void SecurityQuestionsPage::onSecurityQuestionsCheckReplied(const QList<int> &questions)
{
    for (int i = 0; i < questions.size(); ++i) {
        if (i == 0) {
            m_questionCombobox1->setCurrentIndex(questions.at(i));
        } else if (i == 1) {
            m_questionCombobox2->setCurrentIndex(questions.at(i));
        } else if (i == 2) {
            m_questionCombobox3->setCurrentIndex(questions.at(i));
        }
    }
}

void SecurityQuestionsPage::onSetSecurityQuestionsReplied(const QString &errorText)
{
    if (errorText.isEmpty()) {
        // 关闭当前界面时需要先断开信号连接
        disconnect(m_curUser, &User::startSecurityQuestionsCheckReplied, this, &SecurityQuestionsPage::onSecurityQuestionsCheckReplied);
        disconnect(m_curUser, &User::setSecurityQuestionsReplied, this, &SecurityQuestionsPage::onSetSecurityQuestionsReplied);
        Q_EMIT requestBack();
    } else {
        qWarning() << "SetSecurityQuestionsReplied:" << errorText;
    }
}

void SecurityQuestionsPage::addItems(DComboBox *questionCombobox)
{
    questionCombobox->addItem(tr("Please select a question"));
    questionCombobox->addItem(tr("What's the name of the city where you were born?"));
    questionCombobox->addItem(tr("What's the name of the first school you attended?"));
    questionCombobox->addItem(tr("Who do you love the most in this world?"));
    questionCombobox->addItem(tr("What's your favorite animal?"));
    questionCombobox->addItem(tr("What's your favorite song?"));
    questionCombobox->addItem(tr("What's your nickname?"));
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(questionCombobox->model());
    QModelIndex firstIndex = model->index(0, questionCombobox->modelColumn(), questionCombobox->rootModelIndex());
    QStandardItem* firstItem = model->itemFromIndex(firstIndex);
    firstItem->setSelectable(false);
    firstItem->setEnabled(false);
}

bool SecurityQuestionsPage::isContentEmpty(DComboBox *comboBox)
{
    DAlertControl *control = new DAlertControl(comboBox, this);
    if (comboBox->currentIndex() == 0) {
        control->setAlert(true);
        control->showAlertMessage(tr("It cannot be empty"), comboBox, 3000);
    } else {
        control->setAlert(false);
    }

    return comboBox->currentIndex() == 0;
}

bool SecurityQuestionsPage::isContentEmpty(DLineEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("It cannot be empty"), edit, 2000);
    } else {
        edit->setAlert(false);
    }

    return edit->text().isEmpty();
}

bool SecurityQuestionsPage::isSecurityQuestionsEmpty()
{
    return isContentEmpty(m_questionCombobox1) || isContentEmpty(m_questionCombobox2) || isContentEmpty(m_questionCombobox3) ||
            isContentEmpty(m_answerEdit1) || isContentEmpty(m_answerEdit2) || isContentEmpty(m_answerEdit3);
}

QString SecurityQuestionsPage::cryptUserPassword(const QString &password)
{
    /*
        NOTE(kirigaya): Password is a combination of salt and crypt function.
        slat is begin with $6$, 16 byte of random values, at the end of $.
        crypt function will return encrypted values.
     */

    const QString seedchars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedchars.size() - 1); //seedchars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedchars.at(uniform_dist(e1)).toLatin1();
    }

    return crypt(password.toUtf8().data(), salt);
}

bool SecurityQuestionsPage::isAnswersCharactersSizeRight(DLineEdit *edit)
{
    if (edit->text().size() > SECURITY_ANSWERS_CHARACTERS_MAX_SIZE) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Keep the answer under 30 characters"), edit, 2000);
    } else {
        edit->setAlert(false);
    }

    return edit->text().size() <= SECURITY_ANSWERS_CHARACTERS_MAX_SIZE;
}

void SecurityQuestionsPage::checkQuestionDuplicate(int id, int id1, int id2, QWidget *w)
{
    if (id == id1 || id == id2) {
        DAlertControl *control = new DAlertControl(w, this);
        control->setAlert(true);
        control->showAlertMessage(tr("Do not choose a duplicate question please"), w, 3000);
    }
}

void SecurityQuestionsPage::hideAlert(DLineEdit *edit)
{
    if (edit->isAlert()) {
        edit->hideAlertMessage();
        edit->setAlert(false);
    }
}

bool SecurityQuestionsPage::isAllAnswersCharactersSizeRight()
{
    return isAnswersCharactersSizeRight(m_answerEdit1) &&
           isAnswersCharactersSizeRight(m_answerEdit2) &&
           isAnswersCharactersSizeRight(m_answerEdit3);
}

//设置焦点
void SecurityQuestionsPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if (m_questionCombobox1 && !m_questionCombobox1->hasFocus()) {
        m_questionCombobox1->setFocus();
    }
}
