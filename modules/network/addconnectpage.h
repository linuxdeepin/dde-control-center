/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ADDCONNECTPAGE_H
#define ADDCONNECTPAGE_H

#include "listwidgetcontainer.h"

class AddConnectPage : public ListWidgetContainer
{
    Q_OBJECT

public:
    explicit AddConnectPage(QWidget *parent = 0);

private:
    void init();

    int m_currentIndex = 0;
};

#endif // ADDCONNECTPAGE_H
