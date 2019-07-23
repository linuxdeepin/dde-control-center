#include "syncmodule.h"

#include "interfaces/frameproxyinterface.h"
#include "widgets/contentwidget.h"
#include "../../../modules/sync/syncworker.h"
#include "../../../modules/sync/syncmodel.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncModule::SyncModule(dcc::FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , mainWidget(nullptr)
{
}

void SyncModule::initialize()
{

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
    return mainWidget;
}

void SyncModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
