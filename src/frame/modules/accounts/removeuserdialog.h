// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef REMOVEUSERDIALOG_H
#define REMOVEUSERDIALOG_H

#include <ddialog.h>

#include "user.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace accounts {

class RemoveUserDialog : public DDialog
{
    Q_OBJECT
public:
    explicit RemoveUserDialog(const User *user, QWidget *parent = 0);

    bool deleteHome() const;

private:
    bool m_deleteHome;
};

} // namespace accounts
} // namespace dcc

#endif // REMOVEUSERDIALOG_H
