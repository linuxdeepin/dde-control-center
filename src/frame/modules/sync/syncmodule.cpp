#include "syncmodule.h"
#include "syncwidget.h"

using namespace dcc;
using namespace dcc::sync;

SyncModule::SyncModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_moduleWidget(nullptr)
{

}

SyncModule::~SyncModule()
{

}

void SyncModule::initialize()
{

}

const QString SyncModule::name() const
{
    return QStringLiteral("deepin ID");
}

void SyncModule::moduleActive()
{

}

void SyncModule::moduleDeactive()
{

}

void SyncModule::reset()
{

}

ModuleWidget *SyncModule::moduleWidget()
{
    if (!m_moduleWidget) {
        m_moduleWidget = new SyncWidget;
    }

    return m_moduleWidget;
}

void SyncModule::contentPopped(ContentWidget * const w)
{

}
