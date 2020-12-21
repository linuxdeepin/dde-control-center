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
#include "modules/systeminfo/systeminfowork.h"
#include "modules/systeminfo/systeminfomodel.h"
#include "window/mainwindow.h"

#ifndef DISABLE_RECOVERY
#include "systemrestore.h"
#include "backupandrestoremodel.h"
#include "backupandrestoreworker.h"
#endif

using namespace dcc::systeminfo;
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_work(nullptr)
    , m_model(nullptr)
#ifndef DISABLE_RECOVERY
    , m_backupAndRestoreWorker(nullptr)
    , m_backupAndRestoreModel(nullptr)
#endif
    , m_sysinfoWidget(nullptr)
{
    m_frameProxy = frame;
}

SystemInfoModule::~SystemInfoModule()
{
}

void SystemInfoModule::initialize()
{
    if (m_model) {
        delete m_model;
    }
    m_model = new SystemInfoModel(this);
    m_work = new SystemInfoWork(m_model, this);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

#ifndef DISABLE_RECOVERY
    m_backupAndRestoreModel = new BackupAndRestoreModel(this);
    m_backupAndRestoreWorker = new BackupAndRestoreWorker(m_backupAndRestoreModel, this);
    m_backupAndRestoreModel->moveToThread(qApp->thread());
    m_backupAndRestoreWorker->moveToThread(qApp->thread());
#endif

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
    m_sysinfoWidget->setVisible(false);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowAboutNative, this, &SystemInfoModule::onShowAboutNativePage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowVersionProtocol, this, &SystemInfoModule::onVersionProtocolPage);
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowEndUserLicenseAgreement, this, &SystemInfoModule::onShowEndUserLicenseAgreementPage);
#ifndef DISABLE_RECOVERY
    connect(m_sysinfoWidget, &SystemInfoWidget::requestShowRestore, this, &SystemInfoModule::onShowSystemRestore);
#endif
    m_frameProxy->pushWidget(this, m_sysinfoWidget);
    m_sysinfoWidget->setVisible(true);
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

int SystemInfoModule::load(const QString &path)
{
    if (!m_sysinfoWidget) {
        active();
    }

    QListView *list = m_sysinfoWidget->getSystemListViewPointer();
    SystemType type = Default;

    if (!list) {
        return 0;
    }

    if (path == "About This PC") {
        type = AboutThisPC;
    } else if (path == "Edition License") {
        type = EditionLicense;
    } else if (path == "End User License Agreement") {
        type = EndUserLicenseAgreement;
    }

    QModelIndex index = list->model()->index(type, 0);
    switch (type) {
    case AboutThisPC:
        index = list->model()->index(AboutThisPC, 0);
        list->setCurrentIndex(index);
        list->clicked(index);
        break;
    case EditionLicense:
        index = list->model()->index(EditionLicense, 0);
        list->setCurrentIndex(index);
        list->clicked(index);
        break;
    case EndUserLicenseAgreement:
        index = list->model()->index(EndUserLicenseAgreement, 0);
        list->setCurrentIndex(index);
        list->clicked(index);
        break;
    default:
        break;
    }

    return 0;
}

QStringList SystemInfoModule::availPage() const
{
    QStringList availList;
    availList << "About This PC" << "Edition License" << "End User License Agreement";
    return availList;
}

void SystemInfoModule::onShowAboutNativePage()
{
    NativeInfoWidget *w = new NativeInfoWidget(m_model);
    w->setVisible(false);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
    //showActivatorDialog
    connect(w, &NativeInfoWidget::clickedActivator, m_work, &SystemInfoWork::showActivatorDialog);
}

void SystemInfoModule::onVersionProtocolPage()
{
    VersionProtocolWidget *w = new VersionProtocolWidget;
    w->setVisible(false);
    connect(w, &VersionProtocolWidget::loadTextFinished, [ = ](){
        m_frameProxy->pushWidget(this, w);
        w->setVisible(true);
    });
}

void SystemInfoModule::onShowEndUserLicenseAgreementPage()
{
    UserLicenseWidget *w = new UserLicenseWidget;
    w->setVisible(false);
    connect(w, &UserLicenseWidget::loadTextFinished, [ = ](){
        m_frameProxy->pushWidget(this, w);
        w->setVisible(true);
    });
}

#ifndef DISABLE_RECOVERY
void SystemInfoModule::onShowSystemRestore() {
    MainWindow* mainwindow = dynamic_cast<MainWindow*>(m_frameProxy);
    if (mainwindow->getcontentStack().size() == 2 && mainwindow->getcontentStack().at(1).second->objectName() == "SystemRestore") {
        return;
    }
    SystemRestore* restore = new SystemRestore(m_backupAndRestoreModel);
    restore->setVisible(false);
    connect(restore, &SystemRestore::requestSetManualBackupDirectory, m_backupAndRestoreWorker, &BackupAndRestoreWorker::manualBackup);
    connect(restore, &SystemRestore::requestSetSystemBackupDirectory, m_backupAndRestoreWorker, &BackupAndRestoreWorker::systemBackup);
    connect(restore, &SystemRestore::requestManualRestore, m_backupAndRestoreWorker, &BackupAndRestoreWorker::manualRestore);
    connect(restore, &SystemRestore::requestSystemRestore, m_backupAndRestoreWorker, &BackupAndRestoreWorker::systemRestore);

    m_frameProxy->pushWidget(this, restore);
    restore->setVisible(true);
}
#endif
