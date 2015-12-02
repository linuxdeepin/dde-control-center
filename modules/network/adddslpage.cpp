#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>

#include "constants.h"

#include "adddslpage.h"
#include "networkglobal.h"
#include "editlineinput.h"
#include "networkmainwidget.h"

AddDslPage::AddDslPage(const QString &dbusPath, QWidget *parent) :
    ListWidgetContainer(tr("Add PPPoE Connection"), parent),
    m_dbus(new DBusConnectionSession(dbusPath, this))
{
    setRightButtonText(tr("Add"));

    addWidget(new EditLineInput("connection", "id", m_dbus, tr("Name")));
    addWidget(new EditLineInput("pppoe", "username", m_dbus, tr("Username")));
    addWidget(new EditLineInput("pppoe", "service", m_dbus, tr("Service")));
    addWidget(new EditLineInput("pppoe", "password", m_dbus,
                                tr("Password"), EditLineInput::Password));

    connect(this, &AddDslPage::leftButtonClicked, this, [this] {
        m_dbus->Close();

        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if(frame)
            frame->popAllWidget();
    });
    connect(this, &AddDslPage::rightButtonClicked, this, [this] {
        if(m_dbus->Save()) {
            ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

            if(frame)
                frame->popAllWidget();
        }
    });
}
