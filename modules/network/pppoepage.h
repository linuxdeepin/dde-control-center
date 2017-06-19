#ifndef PPPOEPAGE_H
#define PPPOEPAGE_H

#include "contentwidget.h"

#include <QMap>
#include <QPointer>

class QVBoxLayout;
class QPushButton;

namespace dcc {

namespace widgets {
    class SettingsGroup;
    class SwitchWidget;
    class NextPageWidget;
}

namespace network {

class WiredDevice;
class NetworkModel;
class ConnectionEditPage;
class PppoePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit PppoePage(QWidget *parent = 0);
    ~PppoePage();

    void setModel(NetworkModel *model);

signals:
    void requestNextPage(ContentWidget * const w) const;
    void requestCreateConnection(const QString &type, const QString &devicePath) const;
    void requestEditConnection(const QString &devPath, const QString &uuid) const;
    void requestDeleteConnection(const QString &uuid) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;

private slots:
    void createPPPoEConnection();
    void onConnectionListChanged();
    void onConnectionDetailClicked();
    void onConnectionSessionCreated(const QString &devicePath, const QString &sessionPath);
    void onPPPoESelected();
    void onActivateConnectionChanged(const QSet<QString> &conns);

private:
    NetworkModel *m_model;

    widgets::SettingsGroup *m_settingsGrp;
    QPushButton *m_createBtn;
    QMap<widgets::NextPageWidget *, QString> m_connUuid;
    QPointer<ConnectionEditPage> m_editPage;

    QString m_editingUuid;
};

}

}

#endif // PPPOEPAGE_H
