// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deepinidworker.h"
#include "utils.h"

#include <DDBusSender>
#include <dsysinfo.h>

#include <QUrl>
#include <QDesktopServices>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

DCORE_USE_NAMESPACE

DeepinWorker::DeepinWorker(DeepinidModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_deepinIDProxy(new DeepinidDBusProxy(this))
    , m_syncProxy(new SyncDBusProxy(this))
    , m_utcloudInter(new QDBusInterface(SYNC_SERVICE, UTCLOUD_PATH, UTCLOUD_INTERFACE, QDBusConnection::sessionBus(), this))
{

    QDBusConnection::systemBus().connect(LICENSE_SERVICE, LICENSE_PATH, LICENSE_INTERFACE, "LicenseStateChange", this, SLOT(licenseStateChangeSlot()));

    connect(m_deepinIDProxy, &DeepinidDBusProxy::UserInfoChanged, [this](const QVariantMap &userinfo){
        m_model->setUserinfo(userinfo);
    });
}

void DeepinWorker::initData()
{
    licenseStateChangeSlot();
    m_model->setUserinfo(m_deepinIDProxy->userInfo());
}

void DeepinWorker::loginUser()
{
    m_deepinIDProxy->login();
}

void DeepinWorker::logoutUser()
{
    m_deepinIDProxy->logout();
}

void DeepinWorker::openWeb()
{
    QString url = loadCodeURL();
    QUrl::toPercentEncoding(url);
    QDesktopServices::openUrl(QUrl(url));
}

void DeepinWorker::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(&DeepinWorker::getLicenseState, this);
    watcher->setFuture(future);
}

QString DeepinWorker::loadCodeURL()
{
    auto func_getToken = [] {
        QDBusPendingReply<QString> retToken = DDBusSender()
                .service("com.deepin.sync.Daemon")
                .interface("com.deepin.utcloud.Daemon")
                .path("/com/deepin/utcloud/Daemon")
                .method("UnionLoginToken")
                .call();
        retToken.waitForFinished();
        QString token = retToken.value();
        if (token.isEmpty())
            qDebug() << retToken.error().message();
        return token;
    };

    QString oauthURI = "https://login.uniontech.com";
    if (IsCommunitySystem) {
        oauthURI = "https://login.deepin.org";
    }

    if (!qEnvironmentVariableIsEmpty("DEEPINID_OAUTH_URI")) {
        oauthURI = qgetenv("DEEPINID_OAUTH_URI");
    }

    QString url = oauthURI + QString("/oauth2/authorize/registerlogin?autoLoginKey=%1").arg(func_getToken());
    return url;
}

void DeepinWorker::getLicenseState()
{
    if (IsCommunitySystem) {
        m_model->setActivation(true);
        return;
    }

    QDBusInterface licenseInfo(LICENSE_SERVICE, LICENSE_PATH, LICENSE_INTERFACE, QDBusConnection::systemBus());
    if (!licenseInfo.isValid()) {
        qWarning()<< "com.deepin.license error ,"<< licenseInfo.lastError().name();
        return;
    }

    quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
    qDebug() << "authorize result:" << reply;
    m_model->setActivation(reply >= 1 && reply <= 3);
}
