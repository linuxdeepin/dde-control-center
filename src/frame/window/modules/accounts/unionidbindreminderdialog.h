// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef REMINDERDDIALOG_H
#define REMINDERDDIALOG_H

#include <DDialog>

DWIDGET_USE_NAMESPACE

class UnionIDBindReminderDialog : public DDialog
{
    Q_OBJECT
public:
    explicit UnionIDBindReminderDialog(QWidget *parent = nullptr);
    ~UnionIDBindReminderDialog() {}

};

#endif // REMINDERDDIALOG_H
