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

#include <QProcess>
#include <QDBusConnection>
#include <QDesktopServices>

using namespace dcc;
using namespace dcc::unionid;

static QString SYNC_INTERFACE = "com.deepin.sync.Daemon";
const QString UeProgramInterface("com.deepin.userexperience.Daemon");
const QString UeProgramObjPath("/com/deepin/userexperience/Daemon");
UnionidWorker::UnionidWorker(UnionidModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus()))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus()))
{
    m_syncInter->setSync(false, false);
    m_deepinId_inter->setSync(false, false);

    connect(m_deepinId_inter, &DeepinId::UserInfoChanged, m_model, &UnionidModel::setUserinfo, Qt::QueuedConnection);
    connect(m_syncInter, &SyncInter::StateChanged, this, &UnionidWorker::onStateChanged, Qt::QueuedConnection);

    auto req = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.deepinid");
    m_model->setSyncIsValid(req.value() && valueByQSettings<bool>(DCC_CONFIG_FILES, "CloudSync", "AllowCloudSync", false));
}

void UnionidWorker::activate()
{
    m_syncInter->blockSignals(false);
    m_deepinId_inter->blockSignals(false);

    m_model->setUserinfo(m_deepinId_inter->userInfo());
    onStateChanged(m_syncInter->state());
}

void UnionidWorker::deactivate()
{
    m_syncInter->blockSignals(true);
    m_deepinId_inter->blockSignals(true);
}


void UnionidWorker::loginUser()
{
    m_deepinId_inter->Login();
}

void UnionidWorker::logoutUser()
{
    m_deepinId_inter->Logout();
}

void UnionidWorker::setAutoSync(bool autoSync)
{
    m_syncInter->SwitcherSet("enabled", autoSync);
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


