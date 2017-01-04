#ifndef CONNECTIONSESSIONWORKER_H
#define CONNECTIONSESSIONWORKER_H

#include <QObject>

#include <com_deepin_daemon_network_connectionsession.h>

using ConnectionSessionInter = com::deepin::daemon::ConnectionSession;

namespace dcc {

namespace network {

class ConnectionSessionModel;
class ConnectionSessionWorker : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionSessionWorker(const QString &sessionPath, ConnectionSessionModel *model, QObject *parent = 0);

public slots:
    void closeSession();
    void queryAllKeys();
    void saveSettings();
    void changeSettings(const QString &section, const QString &vKey, const QString &data);

private:
    void queryAvailableKeys();

private slots:
    void queryAllKeysCB(QDBusPendingCallWatcher *w);
    void queryAvailableKeysCB(QDBusPendingCallWatcher *w);
    void saveSettingsCB(QDBusPendingCallWatcher *w);

private:
    ConnectionSessionModel *m_connModel;

    ConnectionSessionInter m_sessionInter;
};

}

}

#endif // CONNECTIONSESSIONWORKER_H
