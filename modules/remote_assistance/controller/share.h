#ifndef SHARE_H_FKC6OH8P
#define SHARE_H_FKC6OH8P

#include <QString>
#include "../interface.h"
#include "../dbus/manager.h"
#include "../dbus/server.h"

class ShareController: public IShareController
{
    Q_OBJECT
public:

    ShareController(com::deepin::daemon::Remoting::Manager* manager,
                    com::deepin::daemon::Remoting::Server* server,
                    QObject* p=nullptr);
    virtual ~ShareController() {
        m_server->deleteLater();
    }

    void checkNetworkConnectivity() Q_DECL_OVERRIDE;
    void retry() Q_DECL_OVERRIDE;

public slots:
    bool isSharing() Q_DECL_OVERRIDE;
    void startGenAccessToken() Q_DECL_OVERRIDE;
    void cancel() Q_DECL_OVERRIDE;
    void disconnect() Q_DECL_OVERRIDE;

private slots:
    void onStatusChanged(int);

private:
    QString getPeerID();
    int getStatus();
    int doCheckNetworkConnectivity();

private:
    com::deepin::daemon::Remoting::Manager* m_manager;
    com::deepin::daemon::Remoting::Server* m_server;

    struct ServerStatus {
        enum  {
            Uninitialized,
            Started,
            PeerIDOk,
            PeerIDFailed,
            Sharing,
            Stoped,
            Disconnected,
        };
    };
};

#endif /* end of include guard: SHARE_H_FKC6OH8P */
