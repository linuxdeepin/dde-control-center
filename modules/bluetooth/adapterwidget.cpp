#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>

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
    D_THEME_INIT_WIDGET(AdapterWidget);

    initUI();
}

AdapterWidget::~AdapterWidget()
{
    if(m_info)
        delete m_info;
}

void AdapterWidget::addDevice(BluetoothMainWidget::DeviceInfo *info)
{
    info->adapterInfo = m_info;
    m_deviceItemList->addWidget(info->item);
}

void AdapterWidget::removeDevice(BluetoothMainWidget::DeviceInfo *info)
{
    int index = m_deviceItemList->indexOf(info->item);

    if(index >= 0){
        info->adapterInfo = nullptr;
        m_deviceItemList->removeWidget(index);
    }
}

void AdapterWidget::updateUI()
{
    m_bluetoothName->setText(m_info->name);
    m_bluetoothSwitch->setChecked(m_info->powered);
    m_refreshnndicator->setLoading(m_info->discovering);
    m_deviceItemList->setVisible(m_info->powered);
    m_listWidgetSeparator->setVisible(m_deviceItemList->count() > 0 && m_info->powered);
}

void AdapterWidget::initUI()
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    main_layout->setMargin(0);
    main_layout->setSpacing(0);

    setFixedWidth(DCC::ModuleContentWidth);

    QWidget *name_edit_switch = new QWidget;
    QHBoxLayout *h_layout = new QHBoxLayout(name_edit_switch);
    m_bluetoothName = new NormalLabel(m_info->name);
    ImageNameButton *edit_button = new ImageNameButton("edit");
    m_bluetoothSwitch = new DSwitchButton;

    name_edit_switch->setFixedWidth(DCC::ModuleContentWidth);
    name_edit_switch->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
    m_bluetoothSwitch->setChecked(m_info->powered);

    h_layout->setSpacing(10);
    h_layout->setMargin(0);
    h_layout->addSpacing(10);
    h_layout->addWidget(m_bluetoothName);
    h_layout->addWidget(edit_button);
    h_layout->addStretch(1);
    h_layout->addWidget(m_bluetoothSwitch);
    h_layout->addSpacing(10);

    connect(m_bluetoothSwitch, &DSwitchButton::checkedChanged, this, [this](bool checked){
        m_info->bluetoothDbus->SetAdapterPowered(QDBusObjectPath(m_info->path), checked);
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
            this, [name_edit_switch, edit_name_widget, name_lineEdit, this]{
        name_lineEdit->setText(m_bluetoothName->text());
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
            this, [cancel_button, name_lineEdit, this]{
        cancel_button->click();

        m_info->bluetoothDbus->SetAdapterAlias(QDBusObjectPath(m_info->path), name_lineEdit->text());
    });

    DHeaderLine *headerline = new DHeaderLine;
    ImageNameButton *refresh_button = new ImageNameButton("reload");
    m_refreshnndicator = new DLoadingIndicator;

    refresh_button->setAttribute(Qt::WA_TranslucentBackground);
    m_refreshnndicator->setFixedSize(refresh_button->sizeHint());
    m_refreshnndicator->setWidgetSource(refresh_button);
    m_refreshnndicator->setSmooth(true);
    m_refreshnndicator->setLoading(m_info->discovering);

    headerline->setTitle(tr("Devices nearby"));
    headerline->setLeftMargin(10);
    headerline->setContent(m_refreshnndicator);
    headerline->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);

    m_deviceItemList = new DListWidget;
    m_listWidgetSeparator = new DSeparatorHorizontal;

    m_deviceItemList->setVisible(m_info->powered);
    m_deviceItemList->setStyleSheet(styleSheet());
    m_listWidgetSeparator->hide();
    m_info->widget = this;

    connect(m_deviceItemList, &DListWidget::visibleCountChanged, this, [this](int count){
        m_listWidgetSeparator->setVisible(count > 0 && m_info->powered);
    });
    connect(refresh_button, &ImageNameButton::clicked, this, [this] {
        m_info->bluetoothDbus->RequestDiscovery(QDBusObjectPath(m_info->path));
    });

    main_layout->addWidget(name_edit_switch);
    main_layout->addWidget(edit_name_widget);
    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(headerline);
    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(m_deviceItemList);
    main_layout->addWidget(m_listWidgetSeparator);
    main_layout->addStretch(1);
}
