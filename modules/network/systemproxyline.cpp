#include <libdui/dlineedit.h>
#include <libdui/dlabel.h>
#include <libdui/dboxwidget.h>
#include <libdui/dthememanager.h>

#include "systemproxyline.h"

SystemProxyLine::SystemProxyLine(const QString &title, const QString &proxyType,
                                 DBusNetwork *dbus, QWidget *parent) :
    DHeaderLine(parent)
{
    D_THEME_INIT_WIDGET(SystemProxyWidget)

    setTitle(title);

    DLineEdit *edit_host = new DLineEdit;
    DLineEdit *edit_port = new DLineEdit;
    DLabel *label_port = new DLabel(tr("Port"));
    DHBoxWidget *h_boxwidget = new DHBoxWidget;

    edit_port->setFixedWidth(40);

    ASYN_CALL(dbus->GetProxy(proxyType), {
                  if(args.count() > 1) {
                    edit_host->setText(args[0].toString());
                    edit_port->setText(args[1].toString());
                  }
              }, edit_host, edit_port)

    h_boxwidget->layout()->setSpacing(5);
    h_boxwidget->addWidget(edit_host);
    h_boxwidget->addWidget(label_port);
    h_boxwidget->addWidget(edit_port);

    auto updateProxyInfo = [this, dbus, edit_host, edit_port, proxyType] {
        if(edit_host->text().isEmpty())
            return;

        dbus->SetProxy(proxyType, edit_host->text(),
                       edit_port->text().isEmpty() ? "0" : edit_port->text());
    };

    connect(edit_host, &DLineEdit::textChanged, this, updateProxyInfo);
    connect(edit_port, &DLineEdit::textChanged, this, updateProxyInfo);

    setLeftMargin(15);
    setRightMargin(15);
    setContent(h_boxwidget);
}
