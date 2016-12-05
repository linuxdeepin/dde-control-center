/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef REMOVEUSERDIALOG_H
#define REMOVEUSERDIALOG_H

#include <ddialog.h>

#include "user.h"

DWIDGET_USE_NAMESPACE

class RemoveUserDialog : public DDialog
{
    Q_OBJECT
public:
    explicit RemoveUserDialog(const User *user, QWidget *parent = 0);

    bool deleteHome() const;

private:
    bool m_deleteHome;
};

#endif // REMOVEUSERDIALOG_H
