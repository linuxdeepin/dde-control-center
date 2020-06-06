/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "systeminfomodule.h"
#include "systeminfowidget.h"
#include "nativeinfowidget.h"
#include "versionprotocolwidget.h"
#include "userlicensewidget.h"
#include "systemrestore.h"
#include "backupandrestoremodel.h"
#include "backupandrestoreworker.h"
#include "modules/systeminfo/systeminfowork.h"
#include "modules/systeminfo/systeminfomodel.h"

using namespace dcc::systeminfo;
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
{
    m_frameProxy = frame;
}

SystemInfoModule::~SystemInfoModule()
{
}

void SystemInfoModule::initialize()
{
    m_model = new SystemInfoModel(this);
    m_work = new SystemInfoWork(m_model, this);
    m_backupAndRestoreModel = new BackupAndRestoreModel(this);
    m_backupAndRestoreWorker = new BackupAndRestoreWorker(m_backupAndRestoreModel, this);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
    m_backupAndRestoreModel->moveToThread(qApp->thread());
    m_backupAndRestoreWorker->moveToThread(qApp->thread());

    m_work->activate();
}

void SystemInfoModule::reset()
{
}

void SystemInfoModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void SystemInfoModule::active()
{
    m_sysinfoWidget = new SystemInfoWidget;
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowAboutNative, this, &SystemInfoModule::onShowAboutNativePage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowVersionProtocol, this, &SystemInfoModule::onVersionProtocolPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowEndUserLicenseAgreement, this, &SystemInfoModule::onShowEndUserLicenseAgreementPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowRestore, this, &SystemInfoModule::onShowSystemRestore);
    m_frameProxy->pushWidget(this, m_sysinfoWidget);
    m_sysinfoWidget->setCurrentIndex(0);
}

const QString SystemInfoModule::name() const
{
    return QStringLiteral("systeminfo");
}

const QString SystemInfoModule::displayName() const
{
    return tr("System Info");
}

int SystemInfoModule::load(QString path)
{
    if (!m_sysinfoWidget) {
        active();
    }

    QStringList searchList;
    searchList << "About This PC";

    if (searchList.contains(path)) {
        m_sysinfoWidget->setCurrentIndex(0);
        return 0;
    }

    return -1;
}

QStringList SystemInfoModule::availPage() const
{
    QStringList availList;
    availList << "About This PC";
    return availList;
}

void SystemInfoModule::onShowAboutNativePage()
{
    NativeInfoWidget *w = new NativeInfoWidget(m_model);
    m_frameProxy->pushWidget(this, w);
    //showActivatorDialog
    connect(w, &NativeInfoWidget::clickedActivator, m_work, &SystemInfoWork::showActivatorDialog);
}

void SystemInfoModule::onVersionProtocolPage()
{
    VersionProtocolWidget *w = new VersionProtocolWidget;
    m_frameProxy->pushWidget(this, w);
}

void SystemInfoModule::onShowEndUserLicenseAgreementPage()
{
    UserLicenseWidget *w = new UserLicenseWidget;
    m_frameProxy->pushWidget(this, w);
}

void SystemInfoModule::onShowSystemRestore() {
    SystemRestore* restore = new SystemRestore(m_backupAndRestoreModel);
    connect(restore, &SystemRestore::requestSetManualBackupDirectory, m_backupAndRestoreWorker, &BackupAndRestoreWorker::manualBackup);
    connect(restore, &SystemRestore::requestSetSystemBackupDirectory, m_backupAndRestoreWorker, &BackupAndRestoreWorker::systemBackup);
    connect(restore, &SystemRestore::requestManualRestore, m_backupAndRestoreWorker, &BackupAndRestoreWorker::manualRestore);
    connect(restore, &SystemRestore::requestSystemRestore, m_backupAndRestoreWorker, &BackupAndRestoreWorker::systemRestore);

    m_frameProxy->pushWidget(this, restore);
}
