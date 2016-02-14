/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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

    ShareController(com::deepin::daemon::Remoting::Manager *manager,
                    com::deepin::daemon::Remoting::Server *server,
                    QObject *p = nullptr);
    virtual ~ShareController();

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
    com::deepin::daemon::Remoting::Manager *m_manager;
    com::deepin::daemon::Remoting::Server *m_server;

    struct ServerStatus {
        enum  {
            Uninitialized,
            Started,
            PeerIDOk,
            PeerIDFailed,
            Sharing,
            Stopped,
            Disconnected,
        };
    };
};

#endif /* end of include guard: SHARE_H_FKC6OH8P */
