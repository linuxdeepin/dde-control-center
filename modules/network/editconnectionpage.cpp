/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <libdui/dheaderline.h>
#include <libdui/dtextbutton.h>
#include <libdui/darrowlineexpand.h>
#include <libdui/dlinkbutton.h>
#include <libdui/dthememanager.h>

#include "dtextaction.h"

#include "constants.h"
#include "editconnectionpage.h"
#include "dbus/dbusconnectionsession.h"
#include "networkglobal.h"
#include "networkmainwidget.h"
#include "editlinecombobox.h"
#include "editlinecombobox.h"
#include "editlineinput.h"
#include "editlineswitchbutton.h"
#include "editlinemissingpackage.h"
#include "editlinespinbox.h"

EditConnectionPage::EditConnectionPage(const QString &dbusPath, QWidget *parent) :
    ListWidgetContainer("", parent),
    m_dbus(new DBusConnectionSession(dbusPath, this))
{
    D_THEME_INIT_WIDGET(EditConnectionPage);

    DHeaderLine *line_remove_button = new DHeaderLine;
    DLinkButton *remove_button = new DLinkButton(tr("Delete this network"), line_remove_button);

    line_remove_button->setFixedWidth(DCC::ModuleContentWidth);

    remove_button->adjustSize();
    remove_button->move(10, line_remove_button->height() / 2 - remove_button->height() / 2);


    const QString text_delete = tr("Delete");
    const QString text_save = tr("Save");

    setRightButtonText(text_save);
    boxWidgetLayout()->setSpacing(0);
    boxWidgetLayout()->setContentsMargins(0, 0, 0, 0);

    connect(remove_button, &DLinkButton::clicked, this, [this, remove_button, text_delete] {
        setRightButtonText(text_delete);
        remove_button->setText(tr("Are you sure you want to delete this network?"));
        remove_button->adjustSize();
    });

    connect(this, &EditConnectionPage::rightButtonClicked, this, [this, text_delete] {
        NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

        if(main_widget) {
            if(rightButtonText() == text_delete) {
                main_widget->dbusNetwork()->DeleteConnection(m_dbus->uuid());
                main_widget->popAllWidget();
            } else {
                for(NetworkBaseEditLine *line : findChildren<NetworkBaseEditLine*>()) {
                    if(!line->checkKey()) {
                        DBaseExpand *expand = DCCNetwork::parentObject<DBaseExpand>(line);

                        if(expand) {
                            expand->setExpand(true);
                        }
                    }
                }

                QDBusPendingCallWatcher watcher(m_dbus->Save());

                watcher.waitForFinished();

                if(watcher.reply().arguments().first().toBool()) {
                    main_widget->popAllWidget();
                } else {
                    qDebug() << "Network->EditConnectionPage :Save changes failed, ";
                    qDebug() << "DBus error message:" << watcher.error().message();
                    qDebug() << "DBus ErrorInfo:" << m_dbus->errors();
                }
            }
        }
    });

    connect(this, &EditConnectionPage::leftButtonClicked, this, [this, remove_button, text_delete, text_save] {
        if(rightButtonText() == text_delete) {
            remove_button->setText(tr("Delete this network"));
            remove_button->adjustSize();
            setRightButtonText(text_save);
        } else {
            m_dbus->Close();

            NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

            if(main_widget) {
                main_widget->popAllWidget();
            }
        }
    });

    const QJsonDocument &json_doc = QJsonDocument::fromJson(QString(m_dbus->GetAllKeys()).toUtf8());

    for(const QJsonValue &value : json_doc.array()) {
        const QJsonObject &json_obj = value.toObject();

        if(json_obj["VirtualSection"] != "vs-general") {
            DArrowLineExpand *expand = new DArrowLineExpand;
            DVBoxWidget *boxWidget = new DVBoxWidget;

            expand->setTitle(json_obj["Name"].toString());
            expand->setContent(boxWidget);
            boxWidget->layout()->setSpacing(5);
            boxWidget->layout()->setContentsMargins(0, 5, 0, 5);

            for(const QJsonValue &value : json_obj["Keys"].toArray()) {
                boxWidget->addWidget(getLineByMap(value.toObject().toVariantMap()));
            }

            expand->setExpand(json_obj["Expanded"].toBool());
            expand->headerLine()->setObjectName("expand_headerLine");
            expand->headerLine()->setStyleSheet(expand->headerLine()->styleSheet() + styleSheet());

            addWidget(expand);
        } else {
            for(const QJsonValue &value : json_obj["Keys"].toArray()) {
                addWidget(getLineByMap(value.toObject().toVariantMap()));
            }
        }
    }

    line_remove_button->setVisible(m_dbus->allowDelete());
    setBottomSeparatorVisible(line_remove_button->isVisible());

    addWidget(line_remove_button);
}

EditConnectionPage::~EditConnectionPage()
{
    m_dbus->Close();
}

NetworkBaseEditLine *EditConnectionPage::getLineByMap(const QVariantMap &map)
{
    const QString widget_type = map["WidgetType"].toString();

    NetworkBaseEditLine *line = nullptr;

    if(widget_type == WidgetType::SwitchButton) {
        line = new EditLineSwitchButton(map["Section"].toString(),
                map["Key"].toString(), m_dbus);
    } else if(widget_type == WidgetType::ComboBox) {
        line = new EditLineComboBox(map["Section"].toString(),
                map["Key"].toString(), m_dbus);
    } else if(widget_type == WidgetType::TextInput) {
        line = new EditLineInput(map["Section"].toString(),
                map["Key"].toString(), m_dbus);
    } else if(widget_type == WidgetType::PasswordInput) {
        line = new EditLineInput(map["Section"].toString(),
                map["Key"].toString(),
                m_dbus, EditLineInput::Password);
    } else if(widget_type == WidgetType::FileChooser) {
        line = new EditLineInput(map["Section"].toString(),
                map["Key"].toString(),
                m_dbus, EditLineInput::FileChooser);
    } else if(widget_type == WidgetType::EditableComboBox){
        line = new EditLineComboBox(map["Section"].toString(),
                map["Key"].toString(), m_dbus, true);
    } else if(widget_type == WidgetType::Ipv4Input){
        line = new EditLineInput(map["Section"].toString(),
                map["Key"].toString(),
                m_dbus, EditLineInput::Ipv4Input);
    } else if(widget_type == WidgetType::MissingPackage){
        line = new EditLineMissingPackage(map["Section"].toString(),
                map["Key"].toString(), m_dbus);
    } else if(widget_type == WidgetType::SpinBox){
        line = new EditLineSpinBox(map["Section"].toString(),
                map["Key"].toString(), m_dbus,
                map["MinValue"].toInt(), map["MaxValue"].toInt());
    }

    if(line) {
        line->setAlwaysUpdate(map["AlwaysUpdate"].toBool());
        line->setReadOnly(map["Readonly"].toBool());
        line->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);
    }

    return line;
}

