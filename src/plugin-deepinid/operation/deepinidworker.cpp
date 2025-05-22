// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "deepinidworker.h"
#include "cryptor.h"
#include "utils.h"

#include <DDBusSender>
#include <DNotifySender>
#include <dsysinfo.h>

#include <QUrl>
#include <QDesktopServices>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DeepinIDWorker, "dcc-deepinid-worker");

DCORE_USE_NAMESPACE

static const QString SurPlusError = QStringLiteral("7520");

static void notifyInfo(const QString &reason)
{
    QDBusPendingReply<unsigned int> reply = DUtil::DNotifySender("deepin ID")
                                                .appName("org.deepin.dde.control-center")
                                                .appIcon("deepin-id")
                                                .appBody(reason)
                                                .replaceId(0)
                                                .timeOut(3000)
                                                .actions(QStringList() << "default")
                                                .call();

    reply.waitForFinished();
}

DeepinWorker::DeepinWorker(DeepinidModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_deepinIDProxy(new DeepinidDBusProxy(this))
    , m_syncProxy(new SyncDBusProxy(this))
    , m_utcloudProxy(new UtcloudDBusProxy(this))
    , m_clientService(nullptr)
{
    QDBusConnection::systemBus().connect(LICENSE_SERVICE, LICENSE_PATH, LICENSE_INTERFACE, "LicenseStateChange", this, SLOT(licenseStateChangeSlot()));

    connect(m_deepinIDProxy, &DeepinidDBusProxy::UserInfoChanged, [this](const QVariantMap &userinfo) {
        m_model->setUserinfo(userinfo);
        if (m_model->syncEnabled()) {
            activate();
        }
    });

    connect(m_syncProxy, &SyncDBusProxy::SwitcherChange, this, &DeepinWorker::onSyncSwitcherChange, Qt::QueuedConnection);
    connect(m_syncProxy, &SyncDBusProxy::LastSyncTimeChanged, this, &DeepinWorker::onLastSyncTimeChanged, Qt::QueuedConnection);

    connect(m_utcloudProxy, &UtcloudDBusProxy::SwitcherChange, this, &DeepinWorker::onUtcloudSwitcherChange, Qt::QueuedConnection);
    connect(m_utcloudProxy, &UtcloudDBusProxy::LoginStatus, this, &DeepinWorker::onUtcloudLoginStatusChange, Qt::QueuedConnection);

    m_forgetUrl = utils::forgetPwdURL();
    m_wechatUrl = utils::wechatURL();
}

void DeepinWorker::initData()
{
    // 初始化用户登陆数据
    m_model->setUserinfo(m_deepinIDProxy->userInfo());

    // 获取认证状态
    licenseStateChangeSlot();
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

void DeepinWorker::setFullName(const QString &name)
{
    m_utcloudProxy->SetNickname(name.trimmed());
}

void DeepinWorker::setAutoSync(bool autoSync)
{
    m_syncProxy->SwitcherSet("enabled", autoSync);
}

void DeepinWorker::setSyncSwitcher(const QStringList &keyList, bool enable)
{
    foreach(auto key, keyList) {
        m_syncProxy->SwitcherSet(key, enable);
    }
}

void DeepinWorker::setUtcloudSwitcher(const QString &key, bool enable)
{
    m_utcloudProxy->SwitcherSet(key, enable);
}

bool DeepinWorker::checkPasswdEmpty()
{
    QDBusReply<QString> reply = m_deepinIDProxy->meteInfo();
    if (!reply.isValid()) {
        qCWarning(DeepinIDWorker) << "get mete info error:" << reply.error();
        return false;
    }

    QString data = reply.value();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    return jsonObj["passwordEmpty"].toBool();
}

QVariantMap DeepinWorker::checkPassword(const QString &passwd)
{
    QVariantMap result;
    result["isOk"] = false;
    result["errMsg"] = "";

    QByteArray encryptData;
    if (!Cryptor::RSAPublicEncryptData(m_RSApubkey, passwd, encryptData)) {
        result["errMsg"] = tr("encrypt password failed");
        return result;
    }

    QDBusInterface deepinIf(SYNC_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<QString> reply = deepinIf.call("Checkpwd", QString::fromLocal8Bit(encryptData.toBase64()));
    if (reply.isValid()) {
        m_pwdToken = reply.value();
        result["isOk"] = true;
    } else {
        QString errmsg = reply.error().message();
        qCDebug(DeepinIDWorker) << "check password error:" << errmsg;
        if(errmsg.contains(SurPlusError)) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(errmsg.toUtf8());
            QJsonObject jsonObj = jsonDoc.object();
            QJsonObject dataObj = jsonObj["data"].toObject();
            int count = dataObj["surplus_count"].toInt();
            if (count > 0) {
                result["errMsg"] = tr("Wrong password, %1 chances left").arg(count);
            } else {
                result["errMsg"] = tr("The login error has reached the limit today. You can reset the password and try again.");
            }
        }
    }

    return result;
}

void DeepinWorker::registerPasswd(const QString &passwd)
{
    QByteArray encryptPwd;
    if (Cryptor::RSAPublicEncryptData(m_RSApubkey, passwd, encryptPwd)) {
        QDBusInterface deepinIf(SYNC_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus());
        QDBusReply<void> reply = deepinIf.call("SetPassword", QString::fromLocal8Bit(encryptPwd.toBase64()));
        if (!reply.isValid()) {
            qCWarning(DeepinIDWorker) << "set password error:" << QDBusError::errorString(reply.error().type()) << reply.error();
        }
    } else {
        qCWarning(DeepinIDWorker) << "encrypt password failed";
    }
}

void DeepinWorker::clearData()
{
    QDBusInterface deepinIf(SYNC_SERVICE, UTCLOUD_PATH, UTCLOUD_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<void> reply = deepinIf.asyncCall("Empty");
    if (!reply.isValid()) {
        qCWarning(DeepinIDWorker) << "clear cloud data error:" << reply.error();
    } else {
        qCDebug(DeepinIDWorker) << "clear cloud data success";
    }

    notifyInfo(tr("Operation Successful"));
}

void DeepinWorker::openForgetPasswd()
{
    QString url = QString("%1&time=%2").arg(m_forgetUrl).arg(QDateTime::currentMSecsSinceEpoch());
    qCDebug(DeepinIDWorker) << "forget passwd url:" << url;
    QDBusInterface clientIf(DEEPINCLIENT_SERVICE, DEEPINCLIENT_PATH, DEEPINCLIENT_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<void> reply = clientIf.asyncCall("LoadPage", url);
    if (!reply.isValid()) {
        qCWarning(DeepinIDWorker) << "load page error:" << reply.error();
    }
}

void DeepinWorker::unBindPlatform()
{
    QDBusInterface deepinIf(SYNC_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<void> reply = deepinIf.asyncCall("UnBindPlatform", "wechat");
    if (!reply.isValid()) {
        qCWarning(DeepinIDWorker) << "Unbind platform failed, error: " << reply.error();
    } else {
        qCDebug(DeepinIDWorker) << "Unbind wechat success";
    }

    notifyInfo(tr("Operation Successful"));
}

void DeepinWorker::bindAccount()
{
    QString bindurl;
    QString strsession = getSessionID();
    if (strsession.isEmpty()) {
        bindurl = "";
    } else {
        bindurl = m_wechatUrl;
        bindurl += "&sessionid=";
        bindurl += strsession;
        bindurl += QString("&time=%1").arg(QDateTime::currentMSecsSinceEpoch());
    }

    qCDebug(DeepinIDWorker) << "Init wechat dialog, set bind url: " << bindurl;
    if (m_clientService == nullptr) {
        m_clientService = new QDBusInterface(DEEPINCLIENT_SERVICE, DEEPINCLIENT_PATH, DEEPINCLIENT_INTERFACE, QDBusConnection::sessionBus());
    } else if (!m_clientService->isValid()) {
        delete m_clientService;
        m_clientService = new QDBusInterface(DEEPINCLIENT_SERVICE, DEEPINCLIENT_PATH, DEEPINCLIENT_INTERFACE, QDBusConnection::sessionBus());
    }

    connect(m_clientService, SIGNAL(bindSuccess()), this, SLOT(onBindSuccess()), Qt::UniqueConnection);
    m_clientService->asyncCall("BindAccount", bindurl);
}

void DeepinWorker::onSyncSwitcherChange(const QString &key, bool enable)
{
    if (key == "enabled") {
        m_model->setSyncSwitch(enable);
        return;
    }

    m_model->updateSyncItem(key, enable);
}

void DeepinWorker::onLastSyncTimeChanged(qlonglong lastSyncTime)
{
    m_model->setLastSyncTime(lastSyncTime);
}

void DeepinWorker::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher] {
        // 如果系统为激活状态，并且是中国大陆地区用户，则请求同步状态
        if (m_model->syncEnabled()) {
            activate();
        } else {  // 否则，关闭自动同步
            setAutoSync(false);
        }
        watcher->deleteLater();
    });

    QFuture<void> future = QtConcurrent::run(&DeepinWorker::getLicenseState, this);
    watcher->setFuture(future);
}

void DeepinWorker::onUtcloudSwitcherChange(const QVariantList &args)
{
    if (args.size() >= 2) {
        m_model->updateAppItem(args.at(0).toString(), args.at(1).toBool());
    } else {
        qCWarning(DeepinIDWorker) << "onUtcloudSwitcherChange: args size is wrong";
    }
}

void DeepinWorker::onUtcloudLoginStatusChange(const QVariantList &args)
{
    if (args.size() >= 1) {
        if (args.at(0).toInt() == 4) {
            requestUtCloudDump();
        }
    } else {
        qCWarning(DeepinIDWorker) << "onUtcloudLoginStatusChange: args size is wrong";
    }
}

void DeepinWorker::onBindSuccess()
{
    qCDebug(DeepinIDWorker) << "on bind success";
    notifyInfo(tr("Operation Successful"));

    QDBusInterface deepinIf(SYNC_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<void> reply = deepinIf.asyncCall("FlushUseInfo");
    if (!reply.isValid()) {
        qCWarning(DeepinIDWorker) << "Refresh user info failed, error: " << reply.error();
    }
}

void DeepinWorker::activate()
{
    requestSyncDump();
    requestUtCloudDump();
    onLastSyncTimeChanged(m_syncProxy->lastSyncTime());
    getRSAPubKey();
}

void DeepinWorker::requestSyncDump()
{
    QFutureWatcher<QJsonObject> *watcher = new QFutureWatcher<QJsonObject>(this);
    connect(watcher, &QFutureWatcher<QJsonObject>::finished, this, [=] {
        QJsonObject obj = watcher->result();
        if (obj.isEmpty()) {
            qCWarning(DeepinIDWorker) << "Sync Info is Wrong";
            return;
        }

        qCDebug(DeepinIDWorker) << "Sync dump:" << obj;
        for (auto key : obj.keys()) {
            if (key == "enabled") {
                m_model->setSyncSwitch(obj[key].toBool());
            } else {
                m_model->updateSyncItem(key, obj[key].toBool());
            }
        }
        watcher->deleteLater();
    });

    QFuture<QJsonObject> future = QtConcurrent::run([=]() -> QJsonObject {
        QDBusPendingReply<QString> reply = m_syncProxy->SwitcherDump();
        reply.waitForFinished();
        return QJsonDocument::fromJson(reply.value().toUtf8()).object();
    });

    watcher->setFuture(future);
}

void DeepinWorker::requestUtCloudDump()
{
    QFutureWatcher<QJsonObject> *watcher = new QFutureWatcher<QJsonObject>(this);
    connect(watcher, &QFutureWatcher<QJsonObject>::finished, this, [=] {
        QJsonObject obj = watcher->result();
        if (obj.isEmpty()) {
            qCWarning(DeepinIDWorker) << "Sync Info is Wrong";
            return;
        }
        qCDebug(DeepinIDWorker) << "utcloud dump:" << obj;
        m_model->setSyncSwitch(obj["enabled"].toBool());
        QJsonObject apps = obj["apps"].toObject();

        QList<AppItemData*> appItemList;
        for (auto key : apps.keys()) {
            QJsonObject appData = apps[key].toObject();
            QString name = appData["name"].toString();
            QString displayName = appData["display_name"].toString();
            QString icon = appData["icon"].toString();
            bool enable = appData["enable"].toBool();

            AppItemData *item = new AppItemData;
            item->key = key;
            item->name = displayName.isEmpty() ? name : displayName;
            item->icon = icon;
            item->enable = enable;
            appItemList.append(item);
        }
        m_model->initAppItemList(appItemList);
        watcher->deleteLater();
    });

    QFuture<QJsonObject> future = QtConcurrent::run([=]() -> QJsonObject {
        QDBusPendingReply<QString> reply = m_utcloudProxy->SwitcherDump();
        reply.waitForFinished();
        return QJsonDocument::fromJson(reply.value().toUtf8()).object();
    });

    watcher->setFuture(future);
}

QString DeepinWorker::loadCodeURL()
{
    auto func_getToken = [] {
        QDBusPendingReply<QString> retToken = DDBusSender()
                .service(SYNC_SERVICE)
                .interface(UTCLOUD_INTERFACE)
                .path(UTCLOUD_PATH)
                .method("UnionLoginToken")
                .call();
        retToken.waitForFinished();
        QString token = retToken.value();
        if (token.isEmpty())
            qCWarning(DeepinIDWorker) << retToken.error().message();
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
    qCDebug(DeepinIDWorker) << "open url:" << url;
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
        qCWarning(DeepinIDWorker) << "com.deepin.license error ,"<< licenseInfo.lastError().name();
    } else {
        quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
        m_model->setActivation(reply == 1 || reply == 3);
    }
}

void DeepinWorker::getRSAPubKey()
{
    QDBusInterface deepinIf(SYNC_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus());
    QDBusPendingCall call = deepinIf.asyncCall("GetRSAKey");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=](QDBusPendingCallWatcher *self){
        QDBusPendingReply<QString> reply = *self;
        if(reply.isError()) {
            qCDebug(DeepinIDWorker) << "get rsa key error:" << reply.error();
        }
        else {
            m_RSApubkey = reply.value().toStdString();
        }
        self->deleteLater();
    });
}

QString DeepinWorker::getSessionID()
{
    QString strsession;
    QDBusInterface deepinIdIf(DEEPINID_SERVICE, DEEPINID_PATH, DEEPINID_INTERFACE, QDBusConnection::sessionBus());
    QDBusReply<QByteArray> reply = deepinIdIf.call("Get");
    if (reply.isValid()) {
        auto sessionJson = reply.value();
        auto doc = QJsonDocument::fromJson(sessionJson);
        auto session = doc.object();
        strsession = session.value("SessionID").toString();
    } else {
        qCWarning(DeepinIDWorker) << "get session id error:" << reply.error();
    }

    return strsession;
}
