#include "displaymodule.h"
#include "contentwidget.h"
#include "displaywidget.h"

using namespace dcc;
using namespace dcc::display;

DisplayModule::DisplayModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_displayWorker(nullptr),
      m_displayWidget(nullptr)
{

}

void DisplayModule::initialize()
{
    m_displayWorker = new DisplayWorker;

    m_displayWorker->moveToThread(qApp->thread());
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

    return m_displayWidget;
}
