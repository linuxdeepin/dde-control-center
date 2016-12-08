/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "pincodedialog.h"

#include "labels/largelabel.h"

namespace dcc {
namespace bluetooth {

PinCodeDialog::PinCodeDialog(const QString &pinCode, const bool &cancelable) :
    DDialog(),
    m_pinCodeLabel(new LargeLabel)
{
    setTitle(tr("The PIN for connecting to the Bluetooth device is:"));
    setIcon(QIcon::fromTheme("notification-bluetooth-connected"));

    addContent(m_pinCodeLabel, Qt::AlignTop);

    QStringList btns;
    if (cancelable) {
        btns << tr("Cancel") ;
    }
    btns << tr("OK");
    addButtons(btns);

    setPinCode(pinCode);
}

QString PinCodeDialog::pinCode() const
{
    return m_pinCodeLabel->text();
}

void PinCodeDialog::setPinCode(const QString &pinCode)
{
    m_pinCodeLabel->setText(pinCode);
}

} // namespace bluetooth
} // namespace dcc
