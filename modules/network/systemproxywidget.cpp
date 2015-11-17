#include <QDebug>

#include <libdui/dbuttongrid.h>
#include <libdui/dtextbutton.h>
#include <libdui/dlineedit.h>
#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

#include "constants.h"

#include "systemproxywidget.h"
#include "systemproxyline.h"

SystemProxyWidget::SystemProxyWidget(DBusNetwork *dbus, QWidget *parent) :
    DVBoxWidget(parent)
{
    D_THEME_INIT_WIDGET(SystemProxyWidget)

    DButtonGrid *button_grid = new DButtonGrid(1, 3);

    button_grid->addButtons(QStringList() << tr("None") << tr("Manual") << tr("Auto"));
    button_grid->setFixedSize(DCC::ModuleContentWidth, DUI::CONTENT_HEADER_HEIGHT);

    ASYN_CALL(dbus->GetProxyMethod(), {
                  const QString &method = args[0].toString();

                  if(method == ProxyMethod::None) {
                      button_grid->checkButtonByIndex(0);
                  } else if(method == ProxyMethod::Manual) {
                      button_grid->checkButtonByIndex(1);
                  } else {
                      button_grid->checkButtonByIndex(2);
                  }
              }, button_grid)

    DVBoxWidget *vbox_manual = new DVBoxWidget;
    DVBoxWidget *vbox_auto = new DVBoxWidget;
    DTextButton *button_apply = new DTextButton(tr("Apply system wide"));
    DSeparatorHorizontal *separator_top = new DSeparatorHorizontal;
    QHBoxLayout *layout_button = new QHBoxLayout;

    layout_button->setSpacing(0);
    layout_button->setMargin(0);
    layout_button->addWidget(button_apply, 0, Qt::AlignRight);
    layout_button->addSpacing(15);

    vbox_manual->addWidget(new SystemProxyLine(tr("HTTP Proxy"), ProxyType::HTTP, dbus));
    vbox_manual->addWidget(new SystemProxyLine(tr("HTTPS Proxy"), ProxyType::HTTPS, dbus));
    vbox_manual->addWidget(new SystemProxyLine(tr("FTP Proxy"), ProxyType::FTP, dbus));
    vbox_manual->addWidget(new SystemProxyLine(tr("Socks Proxy"), ProxyType::SOCKS, dbus));

    DHeaderLine *line_autoVbox = new DHeaderLine;
    DLineEdit *edit_autoVbox = new DLineEdit;

    ASYN_CALL(dbus->GetAutoProxy(), {
                 edit_autoVbox->setText(args[0].toString());
              }, edit_autoVbox)

    if(line_autoVbox->layout() && line_autoVbox->layout()->count() > 1)
        delete line_autoVbox->layout()->takeAt(1);
    line_autoVbox->setTitle(tr("Configuration URL"));
    line_autoVbox->setContent(edit_autoVbox);
    line_autoVbox->setLeftMargin(15);
    line_autoVbox->setRightMargin(15);
    line_autoVbox->setStyleSheet(line_autoVbox->styleSheet() + styleSheet());
    vbox_auto->addWidget(line_autoVbox);

    vbox_manual->hide();
    vbox_auto->hide();
    separator_top->hide();
    button_apply->hide();
    button_apply->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(button_grid, &DButtonGrid::buttonCheckedIndexChanged,
            this, [this, button_apply, vbox_manual, vbox_auto, separator_top, dbus](int index) {
        if(m_currentIndex == index)
            return;

        m_currentIndex = index;

        button_apply->setVisible(index > 0);
        vbox_manual->setVisible(index == 1);
        vbox_auto->setVisible(index == 2);
        separator_top->setVisible(index > 0);

        if(index == 0) {
            dbus->SetProxyMethod(ProxyMethod::None);
        }
    });

    connect(edit_autoVbox, &DLineEdit::textChanged,
            this, [this, dbus](const QString &text) {
        dbus->SetAutoProxy(text);
    });

    connect(button_apply, &DTextButton::clicked, this, [this, dbus, button_grid] {
        if(m_currentIndex == 1) {
            dbus->SetProxyMethod(ProxyMethod::Manual);
        } else if(m_currentIndex == 2) {
            dbus->SetProxyMethod(ProxyMethod::Auto);
        }
    });

    addWidget(button_grid);
    addWidget(separator_top);
    addWidget(vbox_manual);
    addWidget(vbox_auto);
    layout()->addLayout(layout_button);
}

