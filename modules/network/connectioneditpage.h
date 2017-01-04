#ifndef CONNECTIONEDITPAGE_H
#define CONNECTIONEDITPAGE_H

#include "contentwidget.h"

#include <QMap>
#include <QPointer>

#include <types/networkerrors.h>

class QVBoxLayout;
class QPushButton;

namespace dcc {

namespace widgets {

class SettingsGroup;
class SettingsItem;

}

namespace network {

class ConnectionSessionModel;
class ConnectionEditPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ConnectionEditPage(QWidget *parent = 0);
    ~ConnectionEditPage();

    void setModel(ConnectionSessionModel *model);

signals:
    void accept() const;
    void requestCancelSession() const;
    void requestChangeSettings(const QString &section, const QString &vKey, const QString &data) const;
    void requestNextPage(ContentWidget * const page) const;

public slots:
    void onDeviceRemoved();

private slots:
    void recreateUI();
    void refershUI();
    void saveFinished(const bool ret);
    void onErrorsChanged(const NetworkErrors &errors);

private:
    void createOptionWidgets(const QString &section, const QJsonObject &keyObject);
    widgets::SettingsItem *createSwitchWidget(const QJsonObject &keyObject, const QJsonObject &infoObject);
    widgets::SettingsItem *createEditWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool password);
    widgets::SettingsItem *createComboWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool editable);
    widgets::SettingsItem *createFileChooserWidget(const QJsonObject &keyObject, const QJsonObject &infoObject);
    widgets::SettingsItem *createSpinBoxWidget(const QJsonObject &keyObject, const QJsonObject &infoObject);

private:
    ConnectionSessionModel *m_sessionModel;

    QPushButton *m_cancelBtn;
    QPushButton *m_acceptBtn;
    QVBoxLayout *m_sectionsLayout;

    QMap<QString, widgets::SettingsGroup *> m_sectionWidgets;
    QMap<QString, QMap<QString, widgets::SettingsItem*>> m_optionWidgets;

    QPointer<ContentWidget> m_nextPage;
    QTimer *m_recreateUITimer;
};

}

}

#endif // CONNECTIONEDITPAGE_H
