#include "networkmodule.h"
#include "networkmodulewidget.h"
#include "contentwidget.h"
#include "modulewidget.h"
#include "networkworker.h"
#include "networkmodel.h"

using namespace dcc::network;

NetworkModule::NetworkModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_networkModel(nullptr),
      m_networkWorker(nullptr),
      m_networkWidget(nullptr)
{
}

void NetworkModule::initialize()
{
    m_networkModel = new NetworkModel;
    m_networkWorker = new NetworkWorker(m_networkModel);

    m_networkModel->moveToThread(qApp->thread());
    m_networkWorker->moveToThread(qApp->thread());
}

void NetworkModule::reset()
{

}

void NetworkModule::moduleActive()
{

}

void NetworkModule::moduleDeactive()
{

}

void NetworkModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString NetworkModule::name() const
{
    return QStringLiteral("network");
}

ModuleWidget *NetworkModule::moduleWidget()
{
    if (m_networkWidget)
        return m_networkWidget;

    m_networkWidget = new NetworkModuleWidget;
    m_networkWidget->setModel(m_networkModel);

    return m_networkWidget;
}
