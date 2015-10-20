#include <QDebug>
#include <QResizeEvent>

#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dloadingindicator.h>

#include "moduleheader.h"
#include "constants.h"
#include "normallabel.h"
#include "imagenamebutton.h"
#include "listwidget.h"
#include "genericlistitem.h"
#include "bluetoothlistitem.h"

#include "bluetoothmainwidget.h"

BluetoothMainWidget::BluetoothMainWidget(QWidget *parent) :
    QFrame(parent),
    m_mainLayout(new QVBoxLayout(this))
{
    initUI();
}

void BluetoothMainWidget::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_deviceList->setFixedHeight(e->size().height() - m_deviceList->y());
}

GenericListItem *getListItem(const QString &name)
{
    GenericListItem *item = new GenericListItem;
    item->setTitle(name);
    item->setShowBgColor(false);

    return item;
}

void BluetoothMainWidget::initUI()
{
    ModuleHeader *header = new ModuleHeader(tr("Bluetooth"), false);

    m_deviceList = new ListWidget;

    updateUI();

    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(m_deviceList);
    m_mainLayout->addStretch(1);
}

void BluetoothMainWidget::updateUI()
{
    for(int i = 0; i < 3; ++i){
        QWidget *device_mainWidget = new QWidget;
        QVBoxLayout *main_layout = new QVBoxLayout(device_mainWidget);

        main_layout->setMargin(0);
        main_layout->setSpacing(0);
        device_mainWidget->setFixedSize(DCC::ModuleContentWidth, 161);

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
                this, [cancel_button, bluetooth_name, name_lineEdit]{
            bluetooth_name->setText(name_lineEdit->text());
            cancel_button->click();
        });

        DHeaderLine *headerline = new DHeaderLine;
        ImageNameButton *refresh_button = new ImageNameButton("reload");
        DLoadingIndicator *refresh_indicator = new DLoadingIndicator;

        refresh_button->setAttribute(Qt::WA_TranslucentBackground);
        refresh_indicator->setFixedSize(refresh_button->sizeHint());
        refresh_indicator->setWidgetSource(refresh_button);
        refresh_indicator->setSmooth(true);
        refresh_indicator->setLoading(true);

        headerline->setTitle(tr("Devices nearby"));
        headerline->setLeftMargin(10);
        headerline->setContent(refresh_indicator);
        headerline->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);

        ListWidget *search_result_list = new ListWidget;
        DSeparatorHorizontal *listWidget_separator = new DSeparatorHorizontal;

        search_result_list->setCheckable(true);
        listWidget_separator->hide();

        connect(search_result_list, &ListWidget::visibleCountChanged, this, [listWidget_separator](int count){
            listWidget_separator->setVisible(count > 0);
        });
        connect(refresh_button, &ImageNameButton::clicked, this, [refresh_indicator]{
            refresh_indicator->setLoading(!refresh_indicator->loading());
        });

        for(int j = 0; j < 3; ++j){
            BluetoothListItem *item = new BluetoothListItem;
            item->setFixedSize(DCC::ModuleContentWidth, DUI::EXPAND_HEADER_HEIGHT);
            item->setTitle("I'm bluetooth");
            search_result_list->addWidget(item);

            connect(item, &BluetoothListItem::checkedChanged, this, [item](bool checked){
                item->setLoading(checked);
            });
        }

        main_layout->addWidget(name_edit_switch);
        main_layout->addWidget(edit_name_widget);
        main_layout->addWidget(new DSeparatorHorizontal);
        main_layout->addWidget(headerline);
        main_layout->addWidget(new DSeparatorHorizontal);
        main_layout->addWidget(search_result_list);
        main_layout->addWidget(listWidget_separator);
        main_layout->addStretch(1);

        m_deviceList->addWidget(device_mainWidget);
    }
}

