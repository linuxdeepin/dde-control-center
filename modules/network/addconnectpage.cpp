/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/dlistwidget.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>
#include <libdui/dbuttonlist.h>

#include "constants.h"

#include "addconnectpage.h"
#include "adddslpage.h"
#include "addvpnpage.h"
#include "genericlistitem.h"
#include "networkglobal.h"
#include "networkmainwidget.h"

DUI_USE_NAMESPACE

AddConnectPage::AddConnectPage(QWidget *parent) :
    ListWidgetContainer(tr("Add Network Connection"), parent)
{
    init();
}

void AddConnectPage::init()
{
    setRightButtonText(tr("Next"));
    setBoxWidgetContentsMargins(0, 5, 0, 5);

    DButtonList *m_button_list = new DButtonList;

    m_button_list->addButtons(QStringList() << tr("PPPoE") << tr("VPN"));
    m_button_list->checkButtonByIndex(0);
    m_button_list->setFixedSize(DCC::ModuleContentWidth, m_button_list->count() * DUI::BUTTON_HEIGHT);

    addWidget(m_button_list);

    connect(m_button_list, &DButtonList::buttonCheckedIndexChanged, this, [this](int index) {
        m_currentIndex = index;
    });

    connect(this, &AddConnectPage::leftButtonClicked, this, [this] {
        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if(frame)
            frame->popAllWidget();
    });

    connect(this, &AddConnectPage::rightButtonClicked, this, [this, m_button_list] {
        NetworkMainWidget *widget = DCCNetwork::parentNetworkMainWidget(this);

        if(widget->stackWidget()->busy())
            return;

        switch (m_currentIndex) {
        case 0:{
            if(widget) {
                ASYN_CALL(widget->dbusNetwork()->CreateConnection(ConnectionType::Pppoe,
                                                                  QDBusObjectPath("/")), {
                              const QString &path = qvariant_cast<QDBusObjectPath>(args[0]).path();

                              if(!widget->stackWidget()->busy())
                                  widget->pushWidget(new AddDslPage(path));
                          }, widget)
            }
            break;
        }
        case 1:{
            if(widget) {
                ASYN_CALL(widget->dbusNetwork()->CreateConnection(ConnectionType::Vpn,
                                                                  QDBusObjectPath("/")), {
                              const QString &path = qvariant_cast<QDBusObjectPath>(args[0]).path();

                              if(!widget->stackWidget()->busy())
                                  widget->pushWidget(new AddVpnPage(path));
                          }, widget)
            }
            break;
        }
        default:
            break;
        }
    });
}

