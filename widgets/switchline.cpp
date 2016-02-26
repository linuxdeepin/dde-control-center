/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "switchline.h"

SwitchLine::SwitchLine(QWidget *parent) : DHeaderLine(parent)
{
    m_button = new DSwitchButton(this);
    connect(m_button, &DSwitchButton::checkedChanged, this, &SwitchLine::checkChanged);
    setContent(m_button);
    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);
    sh->setFixedWidth(width());
    sh->move(0, height() - 2);
}

bool SwitchLine::check() const
{
    return m_button->checked();
}

void SwitchLine::setCheck(bool s)
{
    m_button->setChecked(s);
}

void SwitchLine::setContent(QWidget *content)
{
    DHeaderLine::setContent(content);
}

