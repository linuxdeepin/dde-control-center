#include "displaymodule.h"
#include "contentwidget.h"
#include "displaywidget.h"
#include "resolutiondetailpage.h"
#include "monitorsettingdialog.h"
#include "rotatedialog.h"
#include "recognizedialog.h"

using namespace dcc;
using namespace dcc::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_displayModel(nullptr),
      m_displayWorker(nullptr),
      m_displayWidget(nullptr)
{

}

DisplayModule::~DisplayModule()
{
    m_displayModel->deleteLater();
    m_displayWorker->deleteLater();
}

void DisplayModule::showResolutionDetailPage()
{
    ResolutionDetailPage *page = new ResolutionDetailPage;

    page->setModel(m_displayModel);
    connect(page, &ResolutionDetailPage::requestSetResolution, m_displayWorker, &DisplayWorker::setMonitorResolution);

    m_frameProxy->pushWidget(this, page);
}

void DisplayModule::initialize()
{
    m_displayModel = new DisplayModel;
    m_displayWorker = new DisplayWorker(m_displayModel);

    m_displayWorker->moveToThread(qApp->thread());
    m_displayModel->moveToThread(qApp->thread());
}

void DisplayModule::reset()
{

}

void DisplayModule::moduleActive()
{

}

void DisplayModule::moduleDeactive()
{

}

void DisplayModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString DisplayModule::name() const
{
    return QStringLiteral("display");
}

ModuleWidget *DisplayModule::moduleWidget()
{
    if (m_displayWidget)
        return m_displayWidget;

    m_displayWidget = new DisplayWidget;
    m_displayWidget->setModel(m_displayModel);
    connect(m_displayWidget, &DisplayWidget::showResolutionPage, this, &DisplayModule::showResolutionDetailPage);
    connect(m_displayWidget, &DisplayWidget::requestRotate, [=] { showRotate(m_displayModel->primaryMonitor()); });
    connect(m_displayWidget, &DisplayWidget::requestCustom, this, &DisplayModule::showCustomSettings);

    return m_displayWidget;
}

void DisplayModule::showCustomSettings()
{
    // open all monitors
    for (auto mon : m_displayModel->monitorList())
        m_displayWorker->setMonitorEnable(mon, true);

    MonitorSettingDialog dialog(m_displayModel);

    connect(&dialog, &MonitorSettingDialog::requestMerge, m_displayWorker, &DisplayWorker::mergeScreens);
    connect(&dialog, &MonitorSettingDialog::requestSetPrimary, m_displayWorker, &DisplayWorker::setPrimary);
    connect(&dialog, &MonitorSettingDialog::requestSetMonitorMode, m_displayWorker, &DisplayWorker::setMonitorResolution);
    connect(&dialog, &MonitorSettingDialog::requestSetMonitorBrightness, m_displayWorker, &DisplayWorker::setMonitorBrightness);
    connect(&dialog, &MonitorSettingDialog::requestRecognize, this, &DisplayModule::showRecognize);
    connect(&dialog, &MonitorSettingDialog::requestMonitorRotate, this, &DisplayModule::showRotate);

    // discard or save
    if (dialog.exec() == QDialog::Accepted)
        m_displayWorker->saveChanges();
    else
        m_displayWorker->discardChanges();
}

void DisplayModule::showRecognize()
{
    RecognizeDialog dialog(m_displayModel);
    dialog.exec();
}

void DisplayModule::showRotate(Monitor *mon)
{
    RotateDialog dialog(mon);

    connect(&dialog, &RotateDialog::requestRotate, m_displayWorker, &DisplayWorker::setMonitorRotate);

    dialog.exec();
}
