#ifndef ACCESSPOINTEDITPAGE_H
#define ACCESSPOINTEDITPAGE_H

#include "contentwidget.h"

#include <QMap>

class QVBoxLayout;
class QPushButton;

namespace dcc {

namespace widgets {

class SettingsGroup;

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
    ConnectionSessionModel *m_sessionModel;

    QPushButton *m_cancelBtn;
    QPushButton *m_acceptBtn;
    QVBoxLayout *m_sectionsLayout;

    QMap<QString, widgets::SettingsGroup *> m_sectionWidgets;

    QTimer *m_recreateUITimer;
};

}

}

#endif // ACCESSPOINTEDITPAGE_H
