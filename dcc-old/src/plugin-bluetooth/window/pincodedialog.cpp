//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "pincodedialog.h"

#include <QLabel>
#include <DIconTheme>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

static QList<PinCodeDialog *> Instances;

PinCodeDialog::PinCodeDialog(const QString &pinCode, const bool &cancelable)
    : DDialog()
    , m_pinCodeLabel(new QLabel)
{
    setTitle(tr("The PIN for connecting to the Bluetooth device is:"));
    setIcon(DIconTheme::findQIcon("notification-bluetooth-connected"));

    m_pinCodeLabel->setObjectName("PinCodeText");
    addContent(m_pinCodeLabel, Qt::AlignTop | Qt::AlignCenter);

    QStringList btns;
    if (cancelable) {
        btns << tr("Cancel");
    }
    btns << tr("Confirm");
    addButtons(btns);

    setPinCode(pinCode);
}

PinCodeDialog::~PinCodeDialog()
{
    Instances.removeAll(this);
}

PinCodeDialog *PinCodeDialog::instance(const QString &pinCode, const bool &cancelable)
{
    QList<PinCodeDialog *>::Iterator it = std::find_if(Instances.begin(), Instances.end(), [pinCode](PinCodeDialog *dia) {
        return dia->pinCode() == pinCode;
    });
    if (it != Instances.end()) {
        return *it;
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

PinCodeDialog::PinCodeDialog()
    : PinCodeDialog("", false)
{
}
