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

void UpdateModule::showPage(const QString &pageName)
{
    if (pageName == "available-updates") {
        onPushUpdate();
    }
}

void UpdateModule::onPushUpdate()
{
    if (!m_updatePage) {
        m_updatePage = new UpdateCtrlWidget(m_model);
        m_work->checkForUpdates();

        connect(m_updatePage, &UpdateCtrlWidget::requestDownloadUpdates, m_work, &UpdateWork::downloadAndDistUpgrade);
        connect(m_updatePage, &UpdateCtrlWidget::requestPauseDownload, m_work, &UpdateWork::pauseDownload);
        connect(m_updatePage, &UpdateCtrlWidget::requestResumeDownload, m_work, &UpdateWork::resumeDownload);
        connect(m_updatePage, &UpdateCtrlWidget::requestInstallUpdates, m_work, &UpdateWork::distUpgrade);
        connect(m_updatePage, &UpdateCtrlWidget::suggestReboot, this, [this] {
            QProcess::startDetached("/usr/lib/dde-control-center/reboot-reminder-dialog");
        });
    }

    m_frameProxy->pushWidget(this, m_updatePage);

    // prohibit dde-offline-upgrader from showing while this page is showing.
    QDBusConnection::sessionBus().registerService(OfflineUpgraderService);
}

void UpdateModule::onPushMirrorsView()
{
    if(!m_mirrorsWidget) {
        m_mirrorsWidget = new MirrorsWidget(m_model);

        connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWork::setMirrorSource);
        connect(m_mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work, &UpdateWork::testMirrorSpeed);
    }

    m_frameProxy->pushWidget(this, m_mirrorsWidget);
}

void UpdateModule::onPushSettings()
{
    if (!m_settingsPage) {
        m_settingsPage = new UpdateSettings(m_model);

        connect(m_settingsPage, &UpdateSettings::requestSetAutoUpdate, m_work, &UpdateWork::setAutoDownloadUpdates);
        connect(m_settingsPage, &UpdateSettings::requestShowMirrorsView, this, &UpdateModule::onPushMirrorsView);
    }

    m_frameProxy->pushWidget(this, m_settingsPage);
}

}
}

