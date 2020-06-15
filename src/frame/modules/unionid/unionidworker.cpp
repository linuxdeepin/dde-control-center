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
    , m_syncInter(new SyncInter(SYNC_INTERFACE, "/com/deepin/sync/Daemon", QDBusConnection::sessionBus(), this))
    , m_deepinId_inter(new DeepinId(SYNC_INTERFACE, "/com/deepin/deepinid", QDBusConnection::sessionBus(), this))
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
   /* QList<QVariant> argumentList;

    static QDBusInterface interface("com.deepin.deepinid.Client",
                                        "/com/deepin/deepinid/Client",
                                        "com.deepin.deepinid.Client",
                                        QDBusConnection::sessionBus());
    // TODO: replace with you own id
    argumentList << "163296859db7ff8d72010e715ac06bdf6a2a6f87";
    QDBusMessage msg0 = interface.callWithArgumentList(QDBus::NoBlock, "Register", argumentList);

    argumentList = {};
    // TODO: replace with you own id
    argumentList << "163296859db7ff8d72010e715ac06bdf6a2a6f87";
    argumentList << QStringList{};
    // TODO: replace with you own redirect uri
    argumentList << "https://sync.chinauos.com/oauth/callback";
    // state must gen by server and verify on recive code on server
    argumentList << "random_state";
    QDBusMessage msg = interface.callWithArgumentList(QDBus::NoBlock, "Authorize", argumentList);*/
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

void UnionidWorker::requestAgreementPopup(QString fileName)
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
     //QDesktopServices::openUrl(QUrl("https://www.uniontech.com/agreement/privacy-cn/"));
    /*QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
    if (enabled && (m_dBusUeProgram->IsEnabled() != enabled)) {
        qInfo("suser opened experience project switch.");
        // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
        QString title(tr("UOS Privacy Policy"));
        QString allowContent(tr("Agree and Join User Experience Program"));

        // license路径
        QString content = getLicensePath("/usr/share/deepin-deepinid-client/privacy/deepinid-CN-%1.md", "");

        QProcess *m_process = new QProcess(this);

        auto pathType = "-c";
        QStringList sl;
        sl << "zh_CN" << "zh_TW";
        if (!sl.contains(QLocale::system().name()))
            pathType = "-e";
        m_process->start("dde-license-dialog",
                         QStringList() << "-t" << title << pathType << content << "-a" << allowContent);
        qDebug() << " Deliver content QStringList() = " << "dde-license-dialog"
                 << "-t" << title << pathType << content << "-a" << allowContent;
        connect(m_process, &QProcess::stateChanged, this, [pMainWindow](QProcess::ProcessState state) {
            if (pMainWindow) {
                pMainWindow->setEnabled(state != QProcess::Running);
            } else {
                qDebug() << "setUeProgram pMainWindow is nullptr";
            }
        });
        connect(m_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [ = ] {
            m_process->deleteLater();
        });
    } else {
        if (m_dBusUeProgram->IsEnabled() != enabled) {
            m_dBusUeProgram->Enable(enabled);
            qDebug() << QString("On %1, users close the switch to join the user experience program!").arg(current_date);
        }
    }*/
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


