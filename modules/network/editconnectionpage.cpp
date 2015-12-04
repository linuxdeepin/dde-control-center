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
        remove_button->setText(tr("<font color='yellow'>Are you sure you want to delete this network?</font>"));
    });

    connect(this, &EditConnectionPage::rightButtonClicked, this, [this, text_delete] {
        NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

        if(main_widget) {
            if(rightButtonText() == text_delete) {
                main_widget->dbusNetwork()->DeleteConnection(m_dbus->uuid());
                main_widget->popAllWidget();
            } else {
                if(m_dbus->Save()) {
                    main_widget->popAllWidget();
                } else {
                    qDebug() << "Network->EditConnectionPage :Save changes failed, " << m_dbus->uuid();
                }
            }
        }
    });

    connect(this, &EditConnectionPage::leftButtonClicked, this, [this, remove_button, text_delete, text_save] {
        if(rightButtonText() == text_delete) {
            remove_button->setText(tr("Delete this network"));
            setRightButtonText(text_save);
        } else {
            m_dbus->Close();

            NetworkMainWidget *main_widget = DCCNetwork::parentNetworkMainWidget(this);

            if(main_widget) {
                main_widget->popAllWidget();
            }
        }
    });

    ASYN_CALL(m_dbus->GetAllKeys(), {
                  const QJsonDocument &json_doc = QJsonDocument::fromJson(args[0].toString().toUtf8());

                  for(const QJsonValue &value : json_doc.array()) {
                      const QJsonObject &json_obj = value.toObject();

                      if(!json_obj["VirtualSection"].toString().startsWith("vs-")) {
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

                  addWidget(line_remove_button);
              }, this, line_remove_button)

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
        line = new EditLineSwitchButton(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString());
    } else if(widget_type == WidgetType::ComboBox) {
        line = new EditLineComboBox(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString());
    } else if(widget_type == WidgetType::TextInput) {
        line = new EditLineInput(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString());
    } else if(widget_type == WidgetType::PasswordInput) {
        line = new EditLineInput(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString(), EditLineInput::Password);
    } else if(widget_type == WidgetType::FileChooser) {
        line = new EditLineInput(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString(), EditLineInput::FileChooser);
    } else if(widget_type == WidgetType::EditableComboBox){
        line = new EditLineComboBox(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString(), true);
    } else if(widget_type == WidgetType::Ipv4Input){
        line = new EditLineInput(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString(), EditLineInput::Ipv4Input);
    } else if(widget_type == WidgetType::MissingPackage){
        line = new EditLineMissingPackage(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString());
    } else if(widget_type == WidgetType::SpinBox){
        line = new EditLineInput(map["Section"].toString(), map["Key"].toString(),
                m_dbus, map["Name"].toString(), EditLineInput::SpinBox);
    }

    if(line) {
        line->setAlwaysUpdate(map["AlwaysUpdate"].toBool());
        line->setFixedHeight(DUI::EXPAND_HEADER_HEIGHT);
    }

    return line;
}

