#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/dlistwidget.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>

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
    setCheckable(true);

    GenericListItem *item_pppoe = new GenericListItem;
    GenericListItem *item_vpn = new GenericListItem;

    item_pppoe->setTitle(tr("PPPoE"));
    item_pppoe->setFixedSize(DCC::ModuleContentWidth - 30, DUI::BUTTON_HEIGHT);
    item_vpn->setTitle(tr("VPN"));

    addWidget(item_pppoe);
    //list->addWidget(item_vpn);
    widgetChecke(0);

    connect(this, &AddConnectPage::leftButtonClicked, this, [this] {
        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if(frame)
            frame->popAllWidget();
    });
    connect(this, &AddConnectPage::rightButtonClicked, this, [this] {
        NetworkMainWidget *widget = DCCNetwork::parentNetworkMainWidget(this);

        switch (checkedWidgetIndex()) {
        case 0:{
            if(widget) {
                ASYN_CALL(widget->dbusNetwork()->CreateConnection(ConnectionType::Pppoe, QDBusObjectPath("/")), {
                              const QString &path = qvariant_cast<QDBusObjectPath>(args[0]).path();
                              widget->pushWidget(new AddDslPage(path));
                          }, widget)
            }
            break;
        }
        default:
            break;
        }
    });
}

