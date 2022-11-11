// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef SECURITYKEYWIDGET_H
#define SECURITYKEYWIDGET_H

#include <DPasswordEdit>

#include <QWidget>

class PasswordWidget;

DWIDGET_USE_NAMESPACE

class SecurityKeyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SecurityKeyWidget(const QString &userName = "", QWidget *parent = nullptr);
    ~SecurityKeyWidget();

    QString getInputSecurityKey();
    PasswordWidget* getPasswordWidget();
    bool checkRepeatPassword();
    QString getUserInputSecurityKey();
    void showSecurityKeyAlertMessage(const QString &msg);

private:
    void initWidget();
    void initData();
    QValidator* getRegExp(QObject *obj);
    QString decryptSecurityKey(QString keys);
    int getCursorPlace(QString oldStr, QString newStr);

Q_SIGNALS:
    void notifyDebug(QString);
    void requestGetSecurityKey(QString);

private:
    PasswordWidget *m_passwordWidget;
    DPasswordEdit *m_securityKeyEdit;
    QString m_displayTxt;
    QString m_userName;
    QString m_dbusSecurityKey;
    QString m_lastTxt;
};

#endif // SECURITYKEYWIDGET_H
