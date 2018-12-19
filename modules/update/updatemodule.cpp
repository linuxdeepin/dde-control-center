/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "updatemodule.h"
#include "updateview.h"

namespace dcc{
namespace update{

static const QString OfflineUpgraderService = "com.deepin.dde.OfflineUpgrader";

UpdateModule::UpdateModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_updateView(nullptr),
      m_updatePage(nullptr),
      m_settingsPage(nullptr),
      m_mirrorsWidget(nullptr)
{

}

UpdateModule::~UpdateModule()
{
    m_work->deleteLater();
    m_model->deleteLater();
}

void UpdateModule::initialize()
{
    m_model = new UpdateModel();
    m_work = new UpdateWorker(m_model);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

void UpdateModule::moduleActive()
{
    m_work->activate();
}

void UpdateModule::moduleDeactive()
{
    m_work->deactivate();
}

void UpdateModule::reset()
{

}

void UpdateModule::contentPopped(ContentWidget * const w)
{
    Q_UNUSED(w);
    if(w == m_updatePage) {
        m_updatePage = nullptr;
        QDBusConnection::sessionBus().unregisterService(OfflineUpgraderService);
    }
    else if(w == m_settingsPage)
        m_settingsPage = nullptr;
    else if(w == m_mirrorsWidget)
        m_mirrorsWidget = nullptr;

    w->deleteLater();
}

ModuleWidget *UpdateModule::moduleWidget()
{
    if (!m_updateView)
    {
        m_updateView = new UpdateView;

        connect(m_updateView, SIGNAL(pushUpdate()), this, SLOT(onPushUpdate()));
        connect(m_updateView, SIGNAL(pushMirrors()), this, SLOT(onPushSettings()));
    }

    return m_updateView;
}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

void UpdateModule::showPage(const QString &pageName)
{
    if (pageName == "available-updates")
        onPushUpdate();
}

void UpdateModule::onPushUpdate()
{
    if (!m_updatePage) {
        m_updatePage = new UpdateCtrlWidget(m_model);
        m_work->checkForUpdates();

        connect(m_updatePage, &UpdateCtrlWidget::requestDownloadUpdates, m_work, &UpdateWorker::downloadAndDistUpgrade);
        connect(m_updatePage, &UpdateCtrlWidget::requestPauseDownload, m_work, &UpdateWorker::pauseDownload);
        connect(m_updatePage, &UpdateCtrlWidget::requestResumeDownload, m_work, &UpdateWorker::resumeDownload);
        connect(m_updatePage, &UpdateCtrlWidget::requestInstallUpdates, m_work, &UpdateWorker::distUpgrade);
    }

    m_frameProxy->pushWidget(this, m_updatePage);

    // prohibit dde-offline-upgrader from showing while this page is showing.
    QDBusConnection::sessionBus().registerService(OfflineUpgraderService);
}

void UpdateModule::onPushMirrorsView()
{
    if(!m_mirrorsWidget) {
        m_mirrorsWidget = new MirrorsWidget(m_model);

        m_work->checkNetselect();

        connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWorker::setMirrorSource);
        connect(m_mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work, &UpdateWorker::testMirrorSpeed);
    }

    m_frameProxy->pushWidget(this, m_mirrorsWidget);
}

void UpdateModule::onPushSettings()
{
    if (!m_settingsPage) {
#ifndef DISABLE_SYS_UPDATE_MIRRORS
        m_work->refreshMirrors();
#endif

        m_settingsPage = new UpdateSettings(m_model);

        connect(m_settingsPage, &UpdateSettings::requestSetAutoUpdate, m_work, &UpdateWorker::setAutoDownloadUpdates);
        connect(m_settingsPage, &UpdateSettings::requestShowMirrorsView, this, &UpdateModule::onPushMirrorsView);
        connect(m_settingsPage, &UpdateSettings::requestSetAutoCleanCache, m_work, &UpdateWorker::setAutoCleanCache);
        connect(m_settingsPage, &UpdateSettings::requestSetAutoCheckUpdates, m_work, &UpdateWorker::setAutoCheckUpdates);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
        connect(m_settingsPage, &UpdateSettings::requestSetSourceCheck, m_work, &UpdateWorker::setSourceCheck);
#endif
        connect(m_settingsPage, &UpdateSettings::requestEnableSmartMirror, m_work, &UpdateWorker::setSmartMirror);
    }

    m_frameProxy->pushWidget(this, m_settingsPage);
}

}
}

