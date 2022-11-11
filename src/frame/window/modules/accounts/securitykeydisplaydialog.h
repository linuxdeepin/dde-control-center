// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SECURITYKEYDISPLAYDIALOG_H
#define SECURITYKEYDISPLAYDIALOG_H

#include "modules/accounts/accountsworker.h"

#include <DDialog>
#include <DLabel>

#include <QDBusInterface>

DWIDGET_USE_NAMESPACE

class SecurityKeyDisplayDialog : public DDialog
{
    Q_OBJECT
public:
    explicit SecurityKeyDisplayDialog(dcc::accounts::AccountsWorker *work, DDialog *parent = nullptr);

    void initWidget();
    void initData();
    void updateSecurityKey();
    void saveSecurityKey(QString currentAccount = "");
    void clearSecurityKey();
    void setCurrentAccount(QString account);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QString getRandomString(int length) const;

private:
    void copySecurityKeyToClip();

Q_SIGNALS:
    void notifyClickedCopyIcon();
    void notifySaveSecurityKey(const bool);

public Q_SLOTS:

private:
    DLabel *m_securityKeyDisplayTxt;
    DLabel *m_copyIconLabel;
    QString m_currentAccount;
    dcc::accounts::AccountsWorker *m_work;
};

#endif // SECURITYKEYDISPLAYDIALOG_H
