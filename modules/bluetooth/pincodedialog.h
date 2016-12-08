/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_PINCODEDIALOG_H
#define DCC_BLUETOOTH_PINCODEDIALOG_H

#include <ddialog.h>

DWIDGET_USE_NAMESPACE

class LargeLabel;

namespace dcc {
namespace bluetooth {

class PinCodeDialog : public DDialog
{
    Q_OBJECT
public:
    explicit PinCodeDialog(const QString &pinCode, const bool &cancelable = true);

    QString pinCode() const;
    void setPinCode(const QString &pinCode);

private:
    LargeLabel *m_pinCodeLabel;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_PINCODEDIALOG_H
