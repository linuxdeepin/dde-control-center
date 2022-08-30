// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_BLUETOOTH_PINCODEDIALOG_H
#define DCC_BLUETOOTH_PINCODEDIALOG_H

#include <ddialog.h>
#include <QPointer>

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
    QPointer<dcc::widgets::LargeLabel> m_pinCodeLabel;

    explicit PinCodeDialog();
    explicit PinCodeDialog(const QString &pinCode, const bool &cancelable = true);
    ~PinCodeDialog();
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_PINCODEDIALOG_H
