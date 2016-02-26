/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <libdui/dseparatorhorizontal.h>

#include "constants.h"
#include "moduleheader.h"

#include "mainpanel.h"
#include "buttongroup.h"
#include "button.h"
#include "helper.h"

DUI_USE_NAMESPACE

MainPanel::MainPanel(com::deepin::daemon::Remoting::Manager* manager, QWidget*p): AbstractPanel(tr("Remote Assistance"), p), m_buttongroup(new ButtonGroup)
{
    setObjectName("MainPanel");
    m_manager = manager;

    // addWidget(new DSeparatorHorizontal);
    // m_buttongroup->setGroupTitle(tr("Remote Assistance"));
    addWidget(m_buttongroup->addSeparator());

    Button* button = nullptr;

    button = new Button(tr("Share"), tr("Share your desktop to get remote assistance from other users"));
    button->setNormalIcon(getThemeImage("assistant_share_normal.svg"));
    button->setHoverIcon(getThemeImage("assistant_share_hover.svg"));
    button->setPressedIcon(getThemeImage("assistant_share_press.svg"));
    connect(button, SIGNAL(clicked()), this, SLOT(changeToSharePanel()));
    m_buttongroup->addItem(button)->addSeparator();

    button = new Button(tr("Access"), tr("Access to the desktop shared by other users"));
    button->setNormalIcon(getThemeImage("assistant_access_normal.svg"));
    button->setHoverIcon(getThemeImage("assistant_access_hover.svg"));
    button->setPressedIcon(getThemeImage("assistant_access_press.svg"));
    connect(button, SIGNAL(clicked()), this, SLOT(changeToAccessPanel()));
    m_buttongroup->addItem(button);
}

void MainPanel::emitPanelChanged(ViewPanel v)
{
    emit changePanel(v);
}

void MainPanel::changeToAccessPanel()
{
    emitPanelChanged(ViewPanel::Access);
}

void MainPanel::changeToSharePanel()
{
    emitPanelChanged(ViewPanel::Share);
}
