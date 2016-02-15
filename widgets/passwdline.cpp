/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "passwdline.h"


PasswdLine::PasswdLine(QWidget *parent) : DHeaderLine(parent)
{
    m_passwordEdit = new DPasswordEdit();
    m_passwordEdit->setFixedWidth(160);
    m_passwordEdit->setFixedHeight(DTK_WIDGET_NAMESPACE::BUTTON_HEIGHT);
    connect(m_passwordEdit, &DPasswordEdit::textChanged, this, &PasswdLine::textChanged);
    connect(m_passwordEdit, &DPasswordEdit::focusChanged, this, &PasswdLine::focusChanged);
    setContent(m_passwordEdit);
}

PasswdLine::~PasswdLine()
{
    if (m_warningFrame)
        m_warningFrame->deleteLater();
}

void PasswdLine::hideWarning()
{
    if (!m_warningFrame)
        m_warningFrame = new InputWarningFrame(m_passwordEdit);
    m_warningFrame->hide();
}

void PasswdLine::showWarning(const QString &msg)
{
    if (!m_warningFrame)
        m_warningFrame = new InputWarningFrame(m_passwordEdit);
    m_warningFrame->showWarning(msg);
}

void PasswdLine::setText(const QString &text)
{
    m_passwordEdit->setText(text);
//    m_passwordEdit->setPassword(text);
}

DPasswordEdit *PasswdLine::passwordEdit() const
{
    return m_passwordEdit;
}

QString PasswdLine::text() const
{
    return m_passwordEdit->text();
}
