// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#include "securitykeywidget.h"
#include "passwordwidget.h"
#include "../frame/window/utils.h"

#include <QVBoxLayout>
#include <QRegExpValidator>

SecurityKeyWidget::SecurityKeyWidget(const QString &userName, QWidget *parent)
    : QWidget(parent)
    , m_passwordWidget(new PasswordWidget(81, userName))
    , m_securityKeyEdit(new DPasswordEdit)
    , m_displayTxt("")
    , m_userName(userName)
    , m_lastTxt("")
{
    initWidget();
    initData();
}

SecurityKeyWidget::~SecurityKeyWidget()
{

}

QString SecurityKeyWidget::getInputSecurityKey()
{
    return m_displayTxt.remove(QRegExp("\\-"));
}

void SecurityKeyWidget::initWidget()
{
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *securityKeyLabel = new QLabel(tr("Security Keys") + ":");
    QHBoxLayout *securityKeyLabelLayout = new QHBoxLayout;
    securityKeyLabelLayout->addWidget(securityKeyLabel);
    securityKeyLabelLayout->addSpacing(80);
    m_securityKeyEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
    m_securityKeyEdit->setAlert(true);
    m_securityKeyEdit->setPlaceholderText(tr("Enter the account security key"));
    layout->addLayout(securityKeyLabelLayout);
    layout->addWidget(m_securityKeyEdit);
    layout->addWidget(m_passwordWidget);
    layout->addStretch();
    setLayout(layout);
}

void SecurityKeyWidget::initData()
{
    m_securityKeyEdit->lineEdit()->setValidator(getRegExp(m_securityKeyEdit));
    connect(m_securityKeyEdit->lineEdit(), &QLineEdit::textChanged, this, [=](const QString &txt) {
        if (m_securityKeyEdit->isAlert()) {
            m_securityKeyEdit->hideAlertMessage();
        }
        m_displayTxt = DCC_NAMESPACE::getSecurityKeyDisplayData(txt);
        m_securityKeyEdit->setText(m_displayTxt);
        int place = getCursorPlace(m_lastTxt, m_displayTxt);
        if (place >= 0) {
            m_securityKeyEdit->lineEdit()->setCursorPosition(place);
        }
        if (QString::compare(m_lastTxt, m_displayTxt) != 0) {
            m_lastTxt = m_displayTxt;
        }
    });
}

/* function 正则表达式规则
 * obj 需要进行正则控制的控件
 * return 正则规则
*/
QValidator *SecurityKeyWidget::getRegExp(QObject *obj)
{
    return new QRegExpValidator(QRegExp("^[A-Za-z0-9-]{0,64}$"), obj);
}

//当前删除-的时候，会将光标跳转到最后，原因是输入输出的数据是一样的
int SecurityKeyWidget::getCursorPlace(QString oldStr, QString newStr)
{
    int cursorPlace = -1;
    int lenOld = oldStr.length();
    int lenNew = newStr.length();
    if (lenOld < lenNew) {
        for (int i = 0; i < lenOld; i++) {
            if (oldStr[i] != newStr[i]) {
                cursorPlace = i + 1;
                break;
            }
        }
    } else {
        for (int i = 0; i < lenNew; i++) {
            if (oldStr[i] != newStr[i]) {
                cursorPlace = i;
                break;
            }
        }
    }

    return cursorPlace;
}

PasswordWidget *SecurityKeyWidget::getPasswordWidget()
{
    return m_passwordWidget;
}

bool SecurityKeyWidget::checkRepeatPassword()
{
    if (!m_passwordWidget) {
        return false;
    }
    return m_passwordWidget->checkPassword();
}

QString SecurityKeyWidget::getUserInputSecurityKey()
{
    if (!m_securityKeyEdit) {
        return "";
    }
    return m_securityKeyEdit->text().remove(QRegExp("\\-"));
}

void SecurityKeyWidget::showSecurityKeyAlertMessage(const QString &msg)
{
    if (!m_securityKeyEdit) {
        return;
    }
    m_securityKeyEdit->showAlertMessage(msg);
}
