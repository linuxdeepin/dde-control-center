/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef UPDATEARROWEXPAND_H
#define UPDATEARROWEXPAND_H

#include <libdui/darrowlineexpand.h>
#include <libdui/dimagebutton.h>

#include <QLabel>

DUI_USE_NAMESPACE

class UpdateArrowExpand : public DArrowLineExpand
{
    Q_OBJECT

public:
    explicit UpdateArrowExpand(QWidget *parent = 0);

    void setUpdatableNums(int nums);

signals:
    void configButtonClicked() const;

private:
    QLabel *m_updateCountLabel;
    DImageButton *m_configBtn;
};

#endif // UPDATEARROWEXPAND_H
