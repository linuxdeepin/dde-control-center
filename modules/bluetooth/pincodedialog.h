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


namespace dcc {
namespace widgets {
class LargeLabel;
}

namespace bluetooth {

class PinCodeDialog : public DDialog
{
    Q_OBJECT
public:
    static PinCodeDialog* instance(const QString &pinCode, const bool &cancelable = true);

    QString pinCode() const;
    void setPinCode(const QString &pinCode);

private:
    dcc::widgets::LargeLabel *m_pinCodeLabel;

    explicit PinCodeDialog();
    explicit PinCodeDialog(const QString &pinCode, const bool &cancelable = true);
    ~PinCodeDialog();
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_PINCODEDIALOG_H
