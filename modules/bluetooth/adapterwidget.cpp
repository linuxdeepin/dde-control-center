#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>

#include "constants.h"

#include "imagenamebutton.h"
#include "normallabel.h"

#include "adapterwidget.h"
#include "deviceitemwidget.h"

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

void AdapterWidget::removeConfirm(ConfrimWidget *confrim)
{
    m_activeDeviceList->removeWidget(m_activeDeviceList->indexOf(confrim), false);
}

void AdapterWidget::addConfirm(ConfrimWidget *confirm, BluetoothMainWidget::DeviceInfo *info)
{
    // confirm widget already exists
    if (m_activeDeviceList->indexOf(confirm) != -1)
        return;

    int index = m_activeDeviceList->indexOf(info->item);
    m_activeDeviceList->insertWidget(index + 1, confirm);
}

void AdapterWidget::addDevice(BluetoothMainWidget::DeviceInfo *info)
{
    qDebug() << info->name << info->trusted << info->state;

    info->adapterInfo = m_info;

    m_deviceItemList->addWidget(info->item);
}

void AdapterWidget::addTrustedDevice(BluetoothMainWidget::DeviceInfo *info)
{
    info->adapterInfo = m_info;

    m_activeDeviceList->addWidget(info->item);
    m_activeDeviceExpand->setVisible(m_info->powered);
}

void AdapterWidget::removeDevice(BluetoothMainWidget::DeviceInfo *info, bool isDelete)
{
    int index = m_deviceItemList->indexOf(info->item);

    qDebug() << "remove" << index << info->item;

    if(index >= 0){
        info->adapterInfo = nullptr;
        m_deviceItemList->removeWidget(index, isDelete);
    }
}

void AdapterWidget::removeTrustedDevice(BluetoothMainWidget::DeviceInfo *info)
{
    int index = m_activeDeviceList->indexOf(info->item);

    if(index >= 0){
        info->adapterInfo = nullptr;
        m_activeDeviceList->removeWidget(index);
    }

    qDebug() << "remove trusted device: " << m_activeDeviceList->count();

    m_activeDeviceExpand->setVisible(m_activeDeviceList->count() != 0);
}

void AdapterWidget::updateUI()
{
    QString text = m_info->name;
    QFontMetrics metrics(m_bluetoothName->font());

    m_bluetoothName->setText(metrics.elidedText(text, Qt::ElideRight, 210));
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
    m_bluetoothName = new NormalLabel;
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
        if(m_info->powered != checked)
            m_info->bluetoothDbus->SetAdapterPowered(QDBusObjectPath(m_info->path), checked);
        m_activeDeviceExpand->setVisible(checked && m_activeDeviceList->count());
    });

    QWidget *edit_name_widget = new QWidget;
    QVBoxLayout *editWidget_vLayout = new QVBoxLayout(edit_name_widget);
    DLineEdit *name_lineEdit = new DLineEdit;

    edit_name_widget->hide();
    editWidget_vLayout->setContentsMargins(10, 5, 10, 5);
    editWidget_vLayout->addWidget(name_lineEdit);

    connect(edit_button, &ImageNameButton::clicked,
            this, [name_edit_switch, edit_name_widget, name_lineEdit, this]{
        name_lineEdit->setText(m_info->name);
        name_lineEdit->setFocus();
        name_edit_switch->hide();
        edit_name_widget->show();
    });

    connect(name_lineEdit, &DLineEdit::editingFinished, [this, edit_name_widget, name_lineEdit, name_edit_switch] {
        if (name_lineEdit->text().isEmpty())
            return;

        m_info->bluetoothDbus->SetAdapterAlias(QDBusObjectPath(m_info->path), name_lineEdit->text());

        name_edit_switch->show();
        edit_name_widget->hide();
    });
    connect(name_lineEdit, &DLineEdit::focusChanged, [this, edit_name_widget, name_lineEdit, name_edit_switch] (bool focus) {
        if (!focus)
            name_lineEdit->editingFinished();
    });

    DHeaderLine *headerline = new DHeaderLine;
//    ImageNameButton *refresh_button = new ImageNameButton("waiting");
    DImageButton *refresh_button = new DImageButton;
    refresh_button->setNormalPic(":/dark/images/waiting.png");
    m_refreshnndicator = new DLoadingIndicator;

    refresh_button->setAttribute(Qt::WA_TranslucentBackground);
    m_refreshnndicator->setFixedSize(refresh_button->sizeHint());
    m_refreshnndicator->setWidgetSource(refresh_button);
    m_refreshnndicator->setSmooth(true);
    m_refreshnndicator->setLoading(m_info->discovering);

    headerline->setTitle(tr("Other devices"));
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

    m_activeDeviceList = new DListWidget;
    m_activeDeviceExpand = new DArrowLineExpand;
    m_activeDeviceExpand->setTitle(tr("My devices"));
    m_activeDeviceExpand->setContent(m_activeDeviceList);
    m_activeDeviceExpand->setExpand(true);
    m_activeDeviceExpand->setHidden(true);

    main_layout->addWidget(name_edit_switch);
    main_layout->addWidget(edit_name_widget);
    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(m_activeDeviceExpand);
//    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(headerline);
    main_layout->addWidget(new DSeparatorHorizontal);
    main_layout->addWidget(m_deviceItemList);
    main_layout->addWidget(m_listWidgetSeparator);

    updateUI();
}
