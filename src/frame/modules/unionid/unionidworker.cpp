/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "unionidworker.h"
#include "widgets/utils.h"

#include <QDBusConnection>
#include <QDesktopServices>
#include <QtConcurrent>
#include <DSysInfo>

DCORE_USE_NAMESPACE
using namespace dcc;
using namespace dcc::unionid;
using namespace dcc::cloudsync;

static QString SYNC_INTERFACE = "com.deepin.sync.Daemon";
const QString UeProgramInterface("com.deepin.userexperience.Daemon");
const QString UeProgramObjPath("/com/deepin/userexperience/Daemon");
UnionidWorker::UnionidWorker(UnionidModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus(), this))
{
    m_syncInter->setSync(false, false);
    m_deepinId_inter->setSync(false, false);

    QDBusConnection::sessionBus().connect("com.deepin.sync.Daemon",
                                          "/com/deepin/sync/Daemon",
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged",
                                          "sa{sv}as",
                                          this, SLOT(userInfoChanged(QDBusMessage)));

    m_activeInfo = new QDBusInterface("com.deepin.license",
                                      "/com/deepin/license/Info",
                                      "com.deepin.license.Info",
                                      QDBusConnection::systemBus(),this);

    connect(m_activeInfo, SIGNAL(LicenseStateChange()),this, SLOT(licenseStateChangeSlot()));
    connect(m_deepinId_inter, &DeepinId::UserInfoChanged, m_model, &UnionidModel::setUserinfo, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::StateChanged, this, &UnionidWorker::onStateChanged, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::SwitcherChange, this, &UnionidWorker::onSyncModuleStateChanged, Qt::QueuedConnection);

    auto req = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.deepinid");
    m_model->setSyncIsValid(req.value() && valueByQSettings<bool>(DCC_CONFIG_FILES, "CloudSync", "AllowCloudSync", false));

    licenseStateChangeSlot();

    m_model->setUserinfo(m_deepinId_inter->userInfo());
}

void UnionidWorker::activate()
{
    m_syncInter->blockSignals(false);
    m_deepinId_inter->blockSignals(false);

    m_model->setUserinfo(m_deepinId_inter->userInfo());
    onStateChanged(m_syncInter->state());
    refreshSyncState();
}

void UnionidWorker::deactivate()
{
    m_syncInter->blockSignals(true);
    m_deepinId_inter->blockSignals(true);
}

void UnionidWorker::refreshSyncState()
{
    QFutureWatcher<QJsonObject> *watcher = new QFutureWatcher<QJsonObject>(this);
    connect(watcher, &QFutureWatcher<QJsonObject>::finished, this, [=] {
        watcher->deleteLater();
        QJsonObject obj = std::move(watcher->result());

        if (obj.isEmpty()) {
            qDebug() << "Sync Info is Wrong!";
            return;
        }

        m_model->setEnableSync(obj["enabled"].toBool());

        const std::list<std::pair<SyncType, QStringList>> moduleMap{
            m_model->moduleMap()
        };

        for (auto it = moduleMap.cbegin(); it != moduleMap.cend(); ++it) {
            m_model->setModuleSyncState(it->first, obj[it->second.first()].toBool());
        }
    });

    QFuture<QJsonObject> future = QtConcurrent::run([=]() -> QJsonObject {
        QDBusPendingReply<QString> reply = m_syncInter->SwitcherDump();
        reply.waitForFinished();
        return QJsonDocument::fromJson(reply.value().toUtf8()).object();
    });

    watcher->setFuture(future);
}

void UnionidWorker::signInUser()
{
    //m_deepinId_inter->SignIn();
}

void UnionidWorker::loginUser()
{
    qInfo() << "loginUser";

    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QList<QVariant> argumentList;
    argumentList << "fc8b4f1c34644fd184e002ecdcc6a295";
    argumentList << "com.deepin.dde.ControlCenter";
    argumentList << "/com/deepin/dde/ControlCenter";
    argumentList << "com.deepin.dde.ControlCenter";
    interface.callWithArgumentList(QDBus::NoBlock, "Register", argumentList);

    argumentList = {};
    argumentList << "fc8b4f1c34644fd184e002ecdcc6a295";
    argumentList << QStringList{"base","user.api:contact","user:contact:read"};
    argumentList << "https://uosvip.uniontech.com/account/unionid/callback/uid-management";
    argumentList << "state";
    interface.callWithArgumentList(QDBus::NoBlock, "Authorize", argumentList);
    m_deepinId_inter->Login();
}

void UnionidWorker::logoutUser()
{
    qInfo() << "logoutUser";
    //m_deepinId_inter->Logout();
}

void UnionidWorker::setAutoSync(bool autoSync)
{
    m_syncInter->SwitcherSet("enabled", autoSync);
}

void UnionidWorker::onSyncModuleStateChanged(const QString &module, bool enable)
{
    if (module == "enabled") {
        return m_model->setEnableSync(enable);
    }

    const std::list<std::pair<dcc::cloudsync::SyncType, QStringList>> list = m_model->moduleMap();
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        if (it->second.contains(module)) {
            m_model->setModuleSyncState(it->first, enable);
            break;
        }
    }
}

void UnionidWorker::requestAgreementPopup(const QString &fileName)
{
    auto infos = m_model->userinfo();
    const QString region = infos["Region"].toString();
    if (region != "CN") {
        if(fileName == "servicelabel") {
            QDesktopServices::openUrl(QUrl("https://www.uniontech.com/agreement/agreement-en/"));
        }
        else if (fileName == "privacyLabel") {
            QDesktopServices::openUrl(QUrl("https://www.uniontech.com/agreement/privacy-en/"));
        }
    }
    else {
        if(fileName == "servicelabel") {
            QDesktopServices::openUrl(QUrl("https://www.uniontech.com/agreement/agreement-cn/"));
        }
        else if (fileName == "privacyLabel") {
            QDesktopServices::openUrl(QUrl("https://www.uniontech.com/agreement/privacy-cn/"));
        }
    }
}

void UnionidWorker::requestModifyDialog(QString )
{
    QDesktopServices::openUrl(QUrl("https://account.chinauos.com/account/"));
}


void UnionidWorker::onStateChanged(const IntString &state)
{
    std::pair<qint32, QString> value(state.state, state.description);
    if (UnionidModel::isSyncStateValid(value)) {
        m_model->setSyncState(value);
    }
}

void UnionidWorker::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher,
            &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &UnionidWorker::getLicenseState);
    watcher->setFuture(future);
}

void UnionidWorker::setSync(std::pair<dcc::cloudsync::SyncType, bool> state)
{

    // TODO(justforlxz): Maybe will add screensaver in the future
    // you don't need a multimap.
    const std::list<std::pair<dcc::cloudsync::SyncType, QStringList>> map { m_model->moduleMap() };
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        if (it->first == state.first) {
            for (const QString &value : it->second) {
                m_syncInter->SwitcherSet(value, state.second);
            }
        }
    }
}

void UnionidWorker::getLicenseState()
{
    if (DSysInfo::DeepinDesktop == DSysInfo::deepinType()) {
        m_model->setActivation(true);
        return;
    }

    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qWarning()<< "com.deepin.license error ,"<< licenseInfo.lastError().name();
        return;
    }

    quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
    qDebug() << "authorize result:" << reply;
    m_model->setActivation(reply == 1 || reply == 3);
}

