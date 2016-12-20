#ifndef ACCESSPOINTEDITPAGE_H
#define ACCESSPOINTEDITPAGE_H

#include "contentwidget.h"

#include <QMap>

class QVBoxLayout;
class QPushButton;

namespace dcc {

namespace widgets {

class SettingsGroup;
class SettingsItem;

}

namespace network {

class ConnectionSessionModel;
class AccessPointEditPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit AccessPointEditPage(QWidget *parent = 0);
    ~AccessPointEditPage();

    void setModel(ConnectionSessionModel *model);

signals:
    void requestCancelSession() const;

public slots:
    void onDeviceRemoved();

private slots:
    void recreateUI();
    void refershUI();

private:
    void createOptionWidgets(const QString &section, const QJsonObject &keyObject);
    widgets::SettingsItem *createSwitchWidget(const QJsonObject &keyObject, const QJsonObject &infoObject);
    widgets::SettingsItem *createEditWidget(const QJsonObject &keyObject, const QJsonObject &infoObject);

private:
    ConnectionSessionModel *m_sessionModel;

    QPushButton *m_cancelBtn;
    QPushButton *m_acceptBtn;
    QVBoxLayout *m_sectionsLayout;

    QMap<QString, widgets::SettingsGroup *> m_sectionWidgets;
    QMap<QString, QMap<QString, widgets::SettingsItem*>> m_optionWidgets;

    QTimer *m_recreateUITimer;
};

}

}

#endif // ACCESSPOINTEDITPAGE_H
