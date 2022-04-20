/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
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
