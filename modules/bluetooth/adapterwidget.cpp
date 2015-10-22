#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dloadingindicator.h>

#include "constants.h"

#include "imagenamebutton.h"
#include "normallabel.h"

#include "adapterwidget.h"
#include "bluetoothlistitem.h"

DUI_USE_NAMESPACE

AdapterWidget::AdapterWidget(BluetoothMainWidget::AdapterInfo *info,
                             QWidget *parent) :
    QWidget(parent),
    m_info(info)
{
    initUI(info);
}

DListWidget *AdapterWidget::deviceList() const
{
    return m_deviceList;
}

void AdapterWidget::initUI(BluetoothMainWidget::AdapterInfo *info)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    main_layout->setMargin(0);
    main_layout->setSpacing(0);

    setFixedWidth(DCC::ModuleContentWidth);

    QWidget *name_edit_switch = new QWidget;
    QHBoxLayout *h_layout = new QHBoxLayout(name_edit_switch);
    NormalLabel *bluetooth_name = new NormalLabel(info->name);
    ImageNameButton *edit_button = new ImageNameButton("edit");
    DSwitchButton *bluetooth_switch = new DSwitchButton;

    name_edit_switch->setFixedWidth(DCC::ModuleContentWidth);
    name_edit_switch->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
    bluetooth_switch->setChecked(info->powered);

    h_layout->setSpacing(10);
    h_layout->setMargin(0);
    h_layout->addSpacing(10);
    h_layout->addWidget(bluetooth_name);
    h_layout->addWidget(edit_button);
    h_layout->addStretch(1);
    h_layout->addWidget(bluetooth_switch);
    h_layout->addSpacing(10);

    connect(bluetooth_switch, &DSwitchButton::checkedChanged, this, [this, info](bool checked){
        info->bluetoothDbus->SetAdapterPowered(QDBusObjectPath(info->path), checked);
    });

    QWidget *edit_name_widget = new QWidget;
    QVBoxLayout *editWidget_vLayout = new QVBoxLayout(edit_name_widget);
    QHBoxLayout *editWidget_hLayout = new QHBoxLayout;
    DLineEdit *name_lineEdit = new DLineEdit;
    DTextButton *reset_name_button = new DTextButton(tr("Default Name"));
    DTextButton *cancel_button = new DTextButton(tr("Cancel"));
    DTextButton *apply_button = new DTextButton(tr("Apply"));

    edit_name_widget->hide();
    editWidget_vLayout->setContentsMargins(10, 5, 10, 5);
    editWidget_hLayout->setMargin(0);
    editWidget_vLayout->addWidget(name_lineEdit);
    editWidget_vLayout->addSpacing(5);
    editWidget_vLayout->addLayout(editWidget_hLayout);
    editWidget_hLayout->addWidget(reset_name_button);
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
            this, [cancel_button, bluetooth_name, name_lineEdit, info]{
        cancel_button->click();

        info->bluetoothDbus->SetAdapterAlias(QDBusObjectPath(info->path), name_lineEdit->text());
    });

    DHeaderLine *headerline = new DHeaderLine;
    ImageNameButton *refresh_button = new ImageNameButton("reload");
    DLoadingIndicator *refresh_indicator = new DLoadingIndicator;

    refresh_button->setAttribute(Qt::WA_TranslucentBackground);
    refresh_indicator->setFixedSize(refresh_button->sizeHint());
    refresh_indicator->setWidgetSource(refresh_button);
    refresh_indicator->setSmooth(true);
    refresh_indicator->setLoading(info->discovering);

    headerline->setTitle(tr("Devices nearby"));
    headerline->setLeftMargin(10);
    headerline->setContent(refresh_indicator);
    headerline->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);

    m_deviceList = new DListWidget;
    DSeparatorHorizontal *listWidget_separator = new DSeparatorHorizontal;

    m_deviceList->setCheckable(true);
    listWidget_separator->hide();
    info->listWidget = this;

    connect(m_deviceList, &DListWidget::visibleCountChanged, this, [listWidget_separator](int count){
        listWidget_separator->setVisible(count > 0);
    });
    connect(refresh_button, &ImageNameButton::clicked, this, [info] {
        info->bluetoothDbus->RequestDiscovery(QDBusObjectPath(info->path));
    });

    main_layout->addWidget(name_edit_switch);
    main_layout->addWidget(edit_name_widget);
    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(headerline);
    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(m_deviceList);
    main_layout->addWidget(listWidget_separator);
    main_layout->addStretch(1);
}

