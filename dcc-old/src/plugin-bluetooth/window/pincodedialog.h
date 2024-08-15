//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCC_BLUETOOTH_PINCODEDIALOG_H
#define DCC_BLUETOOTH_PINCODEDIALOG_H

#include <DDialog>

class QLabel;

class PinCodeDialog : public Dtk::Widget::DDialog
{
    Q_OBJECT
public:
    static PinCodeDialog *instance(const QString &pinCode, const bool &cancelable = true);

    QString pinCode() const;
    void setPinCode(const QString &pinCode);

private:
    QLabel *m_pinCodeLabel;

    explicit PinCodeDialog();
    explicit PinCodeDialog(const QString &pinCode, const bool &cancelable = true);
    ~PinCodeDialog();
};

#endif // DCC_BLUETOOTH_PINCODEDIALOG_H
