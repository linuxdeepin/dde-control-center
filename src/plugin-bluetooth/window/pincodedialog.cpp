/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pincodedialog.h"

#include <QLabel>

DWIDGET_USE_NAMESPACE

static QList<PinCodeDialog *> Instances;

PinCodeDialog::PinCodeDialog(const QString &pinCode, const bool &cancelable)
    : DDialog()
    , m_pinCodeLabel(new QLabel)
{
    setTitle(tr("The PIN for connecting to the Bluetooth device is:"));
    setIcon(QIcon::fromTheme("notification-bluetooth-connected"));

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
