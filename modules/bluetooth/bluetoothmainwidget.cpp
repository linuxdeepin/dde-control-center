#include <QDebug>

#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>

#include "moduleheader.h"
#include "constants.h"
#include "normallabel.h"
#include "imagenamebutton.h"

#include "bluetoothmainwidget.h"

BluetoothMainWidget::BluetoothMainWidget(QWidget *parent) :
    QFrame(parent),
    m_mainLayout(new QVBoxLayout(this))
{
    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    initUI();
}

void BluetoothMainWidget::initUI()
{
    ModuleHeader *header = new ModuleHeader(tr("Bluetooth"), false);

    QWidget *name_edit_switch = new QWidget;
    QHBoxLayout *h_layout = new QHBoxLayout(name_edit_switch);
    NormalLabel *bluetooth_name = new NormalLabel("zhang");
    ImageNameButton *edit_button = new ImageNameButton("edit");
    DSwitchButton *bluetooth_switch = new DSwitchButton;

    name_edit_switch->setFixedWidth(DCC::ModuleContentWidth);
    name_edit_switch->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);
    h_layout->setSpacing(10);
    h_layout->setMargin(0);

    h_layout->addSpacing(10);
    h_layout->addWidget(bluetooth_name);
    h_layout->addWidget(edit_button);
    h_layout->addStretch(1);
    h_layout->addWidget(bluetooth_switch);
    h_layout->addSpacing(10);

    QWidget *edit_name_widget = new QWidget;
    QVBoxLayout *editWidget_vLayout = new QVBoxLayout(edit_name_widget);
    QHBoxLayout *editWidget_hLayout = new QHBoxLayout(edit_name_widget);
    DLineEdit *name_lineEdit = new DLineEdit;
    DTextButton *cancel_button = new DTextButton(tr("Cancel"));
    DTextButton *apply_button = new DTextButton(tr("Apply"));

    edit_name_widget->hide();
    editWidget_vLayout->setContentsMargins(10, 5, 10, 5);
    editWidget_hLayout->setMargin(0);
    editWidget_vLayout->addWidget(name_lineEdit);
    editWidget_vLayout->addLayout(editWidget_hLayout);
    editWidget_hLayout->addStretch(1);
    editWidget_hLayout->addWidget(cancel_button);
    editWidget_hLayout->addWidget(apply_button);

    connect(edit_button, &ImageNameButton::clicked,
            this, [name_edit_switch, edit_name_widget, name_lineEdit, bluetooth_name]{
        name_lineEdit->setText(bluetooth_name->text());
        name_lineEdit->setFocus();
        name_edit_switch->hide();
        edit_name_widget->show();
    });

    connect(cancel_button, &DTextButton::clicked,
            this, [name_edit_switch, edit_name_widget]{
        name_edit_switch->show();
        edit_name_widget->hide();
    });

    connect(apply_button, &DTextButton::clicked,
            this, [cancel_button, bluetooth_name, name_lineEdit]{
        bluetooth_name->setText(name_lineEdit->text());
        cancel_button->click();
    });

    DHeaderLine *headerline = new DHeaderLine;
    ImageNameButton *refresh_button = new ImageNameButton("reload");

    headerline->setTitle(tr("Devices nearby"));
    headerline->setLeftMargin(10);
    headerline->setContent(refresh_button);
    headerline->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);

    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(name_edit_switch);
    m_mainLayout->addWidget(edit_name_widget);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(headerline);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addStretch(1);
}

