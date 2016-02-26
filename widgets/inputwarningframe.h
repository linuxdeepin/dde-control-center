/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef INPUTWARNINGFRAME_H
#define INPUTWARNINGFRAME_H

#include <QLabel>
#include <QWidget>
#include <libdui/dthememanager.h>
#include <libdui/darrowrectangle.h>

DUI_USE_NAMESPACE

class InputWarningFrame : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit InputWarningFrame(QWidget *sourceInput);
    void showWarning(const QString &msg);

private:
    QFrame *m_frame = NULL;
    QLabel *m_label = NULL;
    QWidget *m_sourceInput = NULL;
    const int ARROW_LEFT_MARGIN = 16;
};

#endif // INPUTWARNINGFRAME_H
