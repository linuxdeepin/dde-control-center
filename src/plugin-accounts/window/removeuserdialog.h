//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef REMOVEUSERDIALOG_H
#define REMOVEUSERDIALOG_H

#include <ddialog.h>

#include "interface/namespace.h"
#include "src/plugin-accounts/operation/user.h"

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class RemoveUserDialog : public DDialog
{
    Q_OBJECT
public:
    explicit RemoveUserDialog(const User *user, QWidget *parent = nullptr);

    bool deleteHome() const;

private:
    bool m_deleteHome;
};
}

#endif // REMOVEUSERDIALOG_H
