#ifndef ACCESS_H_TDSON91D
#define ACCESS_H_TDSON91D

#include <QObject>
#include <QString>
#include "../interface.h"
#include "../dbus/manager.h"
#include "../dbus/client.h"

class AccessController : public IAccessController
{
    Q_OBJECT
public:
    AccessController(com::deepin::daemon::Remoting::Manager* manager,
                     com::deepin::daemon::Remoting::Client* client,
                     QObject*p=nullptr);

    virtual ~AccessController() {
        m_client->deleteLater();
    }

    void checkNetworkConnectivity() Q_DECL_OVERRIDE;

public slots:
    void connect(QString) Q_DECL_OVERRIDE;
    void disconnect() Q_DECL_OVERRIDE;
    void retry() Q_DECL_OVERRIDE;

private slots:
    void onStatusChanged(int);

public:
    bool isAlreadyConnected() Q_DECL_OVERRIDE;
    void initStatus() Q_DECL_OVERRIDE;

private:
    int doCheckNetworkConnectivity();
    int getStatus();

private:
    struct ClientStatus {
        enum {
            Unintialized,
            Started,
            Stopped,
            PageReady,
            Connecting,
            ConnectOk,
            ConnectServerFailed,
            InvalidToken,
            Disconnected
        };
    };

    com::deepin::daemon::Remoting::Manager* m_manager;
    com::deepin::daemon::Remoting::Client* m_client;
    QString m_accessToken;
};

#endif /* end of include guard: ACCESS_H_TDSON91D */
