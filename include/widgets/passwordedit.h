// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DCC_PASSWORDEDIT_H
#define DCC_PASSWORDEDIT_H

#include <DPasswordEdit>

namespace dcc {
namespace widgets {

class PasswordEdit : public DTK_WIDGET_NAMESPACE::DPasswordEdit
{
    Q_OBJECT
public:
    explicit PasswordEdit(QWidget *parent = 0);

Q_SIGNALS:
    void getNewPassWdLevel(QString newPasswd);
protected:
    bool eventFilter(QObject *, QEvent *) override;
};

}
} // namespace dcc

#endif // DCC_TRANSLUCENTFRAME_H
