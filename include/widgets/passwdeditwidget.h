// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef PASSWDEDITWIDGET_H
#define PASSWDEDITWIDGET_H

#include "widgets/lineeditwidget.h"

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace widgets {
class PasswdEditWidget : public LineEditWidget
{
    Q_OBJECT
public:
    explicit PasswdEditWidget(QFrame *parent = nullptr);

    void setSwitchBtnVisible(bool visible);

private Q_SLOTS:
    void switchMode();
    void updateBtnPic();

private:
    DImageButton *m_switchBtn;
};
}
}

#endif // PASSWDEDITWIDGET_H
