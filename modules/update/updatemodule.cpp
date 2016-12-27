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

    if (m_updateView)
        m_updateView->deleteLater();
}

void UpdateModule::initialize()
{
    m_model = new UpdateModel();
    m_work = new UpdateWork(m_model);

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

void UpdateModule::onPushUpdate()
{
    m_work->checkForUpdates();

    // prohibit dde-offline-upgrader from showing while this page is showing.
    QDBusConnection::sessionBus().registerService(OfflineUpgraderService);

    if (!m_updatePage) {
        m_updatePage = new UpdateCtrlWidget(m_model);

        connect(m_updatePage, &UpdateCtrlWidget::requestDownloadUpdates, m_work, &UpdateWork::downloadUpdates);
        connect(m_updatePage, &UpdateCtrlWidget::requestPauseDownload, m_work, &UpdateWork::pauseDownload);
        connect(m_updatePage, &UpdateCtrlWidget::requestResumeDownload, m_work, &UpdateWork::resumeDownload);
        connect(m_updatePage, &UpdateCtrlWidget::requestInstallUpdates, [this] {
            QDBusConnection::sessionBus().unregisterService(OfflineUpgraderService);

            QProcess::startDetached("/usr/lib/deepin-daemon/dde-offline-upgrader");
        });
    }

    m_frameProxy->pushWidget(this, m_updatePage);
}

void UpdateModule::onPushMirrorsView()
{
    m_work->testMirrorSpeed();

    if(!m_mirrorsWidget) {
        m_mirrorsWidget = new MirrorsWidget(m_model);

        connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWork::setMirrorSource);
    }

    m_frameProxy->pushWidget(this, m_mirrorsWidget);
}

void UpdateModule::onPushSettings()
{
    if (!m_settingsPage) {
        m_settingsPage = new UpdateSettings(m_model);

        connect(m_settingsPage, &UpdateSettings::requestSetAutoUpdate, m_work, &UpdateWork::setAutoUpdate);
        connect(m_settingsPage, &UpdateSettings::requestShowMirrorsView, this, &UpdateModule::onPushMirrorsView);
    }

    m_frameProxy->pushWidget(this, m_settingsPage);
}

}
}

