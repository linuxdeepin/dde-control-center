// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "pincodedialog.h"

#include "widgets/labels/largelabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace bluetooth {

static QList<PinCodeDialog*> Instances;

PinCodeDialog::PinCodeDialog(const QString &pinCode, const bool &cancelable) :
    DDialog(),
    m_pinCodeLabel(new dcc::widgets::LargeLabel(this))
{
    setTitle(tr("The PIN for connecting to the Bluetooth device is:"));
    setIcon(QIcon::fromTheme("notification-bluetooth-connected"));

    m_pinCodeLabel->setObjectName("PinCodeText");
    addContent(m_pinCodeLabel, Qt::AlignTop | Qt::AlignCenter);

    QStringList btns;
    if (cancelable) {
        btns << tr("Cancel") ;
    }
    btns << tr("Confirm");
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
    if (m_pinCodeLabel) {
        return m_pinCodeLabel->text();
    } else {
        return "";
    }

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
