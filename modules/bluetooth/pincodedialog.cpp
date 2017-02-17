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

using namespace dcc::widgets;

namespace dcc {
namespace bluetooth {

static QList<PinCodeDialog*> Instances;

PinCodeDialog::PinCodeDialog(const QString &pinCode, const bool &cancelable) :
    DDialog(),
    m_pinCodeLabel(new dcc::widgets::LargeLabel)
{
    setTitle(tr("The PIN for connecting to the Bluetooth device is:"));
    setIcon(QIcon::fromTheme("notification-bluetooth-connected"));

    m_pinCodeLabel->setObjectName("PinCodeText");
    addContent(m_pinCodeLabel, Qt::AlignTop);

    QStringList btns;
    if (cancelable) {
        btns << tr("Cancel") ;
    }
    btns << tr("OK");
    addButtons(btns);

    setPinCode(pinCode);
}

PinCodeDialog::~PinCodeDialog()
{
    Instances.removeAll(this);
}

PinCodeDialog* PinCodeDialog::instance(const QString &pinCode, const bool &cancelable)
{
    for (PinCodeDialog *dia : Instances) {
        if (dia->pinCode() == pinCode) {
            return dia;
        }
    }

    PinCodeDialog *dia = new PinCodeDialog(pinCode, cancelable);
    Instances.append(dia);

    return dia;
}

QString PinCodeDialog::pinCode() const
{
    return m_pinCodeLabel->text();
}

void PinCodeDialog::setPinCode(const QString &pinCode)
{
    m_pinCodeLabel->setText(pinCode);
}

PinCodeDialog::PinCodeDialog() :
    PinCodeDialog("", false)
{

}

} // namespace bluetooth
} // namespace dcc
