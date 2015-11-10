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

DUI_USE_NAMESPACE

AddConnectPage::AddConnectPage(QWidget *parent) :
    DVBoxWidget(parent)
{
    D_THEME_INIT_WIDGET(AddConnectPage)

    init();
}

void AddConnectPage::init()
{
    DHeaderLine *title = new DHeaderLine;

    title->setTitle("Add Network Connection");
    title->setFixedWidth(DCC::ModuleContentWidth);

    QHBoxLayout *button_layout = new QHBoxLayout;
    DTextButton *button_cancel = new DTextButton(tr("Cancel"));
    DTextButton *button_next = new DTextButton(tr("Next"));

    button_layout->setMargin(0);
    button_layout->setSpacing(5);
    button_layout->addStretch(1);
    button_layout->addWidget(button_cancel);
    button_layout->addWidget(button_next);
    button_layout->addSpacing(15);

    DListWidget *list = new DListWidget;
    GenericListItem *item_pppoe = new GenericListItem;
    GenericListItem *item_vpn = new GenericListItem;

    item_pppoe->setTitle(tr("PPPoE"));
    item_vpn->setTitle(tr("VPN"));
    list->setCheckable(true);
    list->setStyleSheet(styleSheet());
    list->setItemSize(DCC::ModuleContentWidth, DUI::BUTTON_HEIGHT);
    list->addWidget(item_pppoe);
    //list->addWidget(item_vpn);
    list->setChecked(0, true);

    addWidget(title);
    addWidget(new DSeparatorHorizontal);
    addWidget(list);
    addWidget(new DSeparatorHorizontal);
    layout()->addSpacing(10);
    layout()->addLayout(button_layout);

    connect(button_cancel, &DTextButton::clicked, this, [this] {
        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if(frame)
            frame->popAllWidget();
    });
    connect(button_next, &DTextButton::clicked, this, [this, list] {
        NetworkMainWidget *widget = DCCNetwork::parentNetworkMainWidget(this);

        switch (list->firstChecked()) {
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

