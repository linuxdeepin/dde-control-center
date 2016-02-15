/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef INPUTLINE_H
#define INPUTLINE_H

#include <QWidget>
#include "inputwarningframe.h"
#include "dlineedit.h"
#include "dheaderline.h"

DWIDGET_USE_NAMESPACE

class InputLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit InputLine(QWidget *parent = 0);
    ~InputLine();
    void hideWarning();
    void showWarning(const QString &msg);
    void setText(const QString &text);
    QString text() const;
    DLineEdit *lineEdit() const;

signals:
    void textChanged(const QString &text);
    void focusChanged(bool focus);

private:
    void setContent(QWidget *content);

private:
    InputWarningFrame *m_warningFrame = NULL;
    DLineEdit *m_lineEdit = NULL;
};

#endif // INPUTLINE_H
