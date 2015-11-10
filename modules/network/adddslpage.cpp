#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/dseparatorhorizontal.h>

#include "constants.h"

#include "adddslpage.h"
#include "networkglobal.h"
#include "editlineinput.h"

AddDslPage::AddDslPage(const QString &dbusPath, QWidget *parent) :
    DVBoxWidget(parent),
    m_dbus(new DBusConnectionSession(dbusPath))
{
    DHeaderLine *title = new DHeaderLine;

    title->setTitle("Add PPPoE Connection");
    title->setFixedWidth(DCC::ModuleContentWidth);

    QHBoxLayout *button_layout = new QHBoxLayout;
    DTextButton *button_cancel = new DTextButton(tr("Cancel"));
    DTextButton *button_add = new DTextButton(tr("Add"));

    button_layout->setMargin(0);
    button_layout->setSpacing(5);
    button_layout->addStretch(1);
    button_layout->addWidget(button_cancel);
    button_layout->addWidget(button_add);
    button_layout->addSpacing(15);

    DVBoxWidget *main_widget = new DVBoxWidget;

    main_widget->layout()->setContentsMargins(15, 5, 15, 5);
    main_widget->layout()->setSpacing(5);
    main_widget->addWidget(new EditLineInput("connection", "id", m_dbus, tr("Name")));
    main_widget->addWidget(new EditLineInput("pppoe", "username", m_dbus, tr("Username")));
    main_widget->addWidget(new EditLineInput("pppoe", "service", m_dbus, tr("Service")));
    main_widget->addWidget(new EditLineInput("pppoe", "password", m_dbus,
                                             tr("Password"), BaseLineEditType::Password));

    addWidget(title);
    addWidget(new DSeparatorHorizontal);
    addWidget(main_widget);
    addWidget(new DSeparatorHorizontal);
    layout()->addSpacing(10);
    layout()->addLayout(button_layout);

    connect(button_cancel, &DTextButton::clicked, this, [this] {
        m_dbus->Close();

        ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

        if(frame)
            frame->popAllWidget();
    });
    connect(button_add, &DTextButton::clicked, this, [this] {
        if(m_dbus->Save()) {
            ScrollFrame *frame = DCCNetwork::parentNetworkMainWidget(this);

            if(frame)
                frame->popAllWidget();
        }
    });
}
