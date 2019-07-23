#include "syncmodule.h"

#include "interfaces/frameproxyinterface.h"
#include "../../../modules/sync/syncworker.h"
#include "../../../modules/sync/syncmodel.h"

#include "syncwidget.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncModule::SyncModule(dcc::FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_mainWidget(nullptr)
{
}

void SyncModule::initialize()
{
    m_mainWidget = new SyncWidget;
}

void SyncModule::reset() {}

const QString SyncModule::name() const
{
    return QStringLiteral("cloudsync");
}

void SyncModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *SyncModule::moduleWidget()
{
    return m_mainWidget;
}

void SyncModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
