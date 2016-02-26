/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QString>

namespace NetworkConnectivity
{
enum {
    Unknown,
    Connected,
    Disconnected,
};
}

class Connectable: public QObject
{
    Q_OBJECT
public:
    Connectable(QObject *p): QObject(p) {}
    virtual ~Connectable() {}
    virtual void checkNetworkConnectivity() = 0;
    virtual void retry() = 0;

signals:
    void disconnected();
    void noNetwork();
    void stopped();
};

namespace AccessError
{
enum AccessError {
    InvalidToken,
    ConnectServerFailed,
};
}

Q_DECLARE_FLAGS(AccessErrors, AccessError::AccessError)

class IAccessController: public Connectable
{
    Q_OBJECT
public:
    IAccessController(QObject *p): Connectable(p) {}

    virtual void initStatus() = 0;
    virtual bool isAlreadyConnected() = 0;

public slots:
    virtual void connect(QString) = 0;
    virtual void disconnect() = 0;

signals:
    void stopped();
    void connecting();
    void connected();
    void connectFailed(AccessErrors);
};

class IShareController : public Connectable
{
    Q_OBJECT
public:
    IShareController(QObject *p): Connectable(p) {}

public slots:
    virtual bool isSharing() = 0;
    virtual void startGenAccessToken() = 0;
    virtual void cancel() = 0;
    virtual void disconnect() = 0;

signals:
    void generatingAccessToken();
    void sharing();
    void genAccessTokenSuccessed(QString);
    void genAccessTokenFailed();
};

#endif // INTERFACE_H

