/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef PASSWDLINE_H
#define PASSWDLINE_H

#include <QWidget>
#include "inputwarningframe.h"
#include "libdui/dpasswordedit.h"
#include "libdui/dheaderline.h"
#include "libdui/dconstants.h"

DUI_USE_NAMESPACE

class PasswdLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit PasswdLine(QWidget *parent = 0);
    ~PasswdLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DPasswordEdit *passwordEdit() const;

signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DPasswordEdit *m_passwordEdit = NULL;
};

#endif // PASSWDLINE_H
