// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPINIDWORKER_H
#define DEEPINIDWORKER_H

#include "deepinidmodel.h"
#include "deepiniddbusproxy.h"
#include "syncdbusproxy.h"

class DeepinWorker : public QObject
{
    Q_OBJECT
public:
    explicit DeepinWorker(DeepinidModel *model, QObject *parent = nullptr);

    void initData();


public Q_SLOTS:
    void loginUser();
    void logoutUser();
    void openWeb();

    void licenseStateChangeSlot();

private:
    QString loadCodeURL();
    void getLicenseState();

private:
    DeepinidModel *m_model;
    DeepinidDBusProxy *m_deepinIDProxy;
    SyncDBusProxy *m_syncProxy;
    QDBusInterface *m_utcloudInter;
};

#endif // DEEPINIDWORKER_H
