#include "updatemodule.h"
#include "updateview.h"

UpdateModule::UpdateModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_updateView(nullptr),
      m_updatePage(nullptr)
{

}

UpdateModule::~UpdateModule()
{
    if (m_updateView)
        m_updateView->deleteLater();
}

void UpdateModule::initialize()
{

}

void UpdateModule::moduleActive()
{
//    qDebug() << "update active";
}

void UpdateModule::moduleDeactive()
{
//    qDebug() << "update deactive";
}

void UpdateModule::contentPopped(ContentWidget * const w)
{
    Q_UNUSED(w);
}

ModuleWidget *UpdateModule::moduleWidget()
{
    if (!m_updateView)
    {
        m_updateView = new UpdateView;

        connect(m_updateView, &UpdateView::showUpdateWidget, this, &UpdateModule::showUpdatePage);
    }

    return m_updateView;
}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

void UpdateModule::showUpdatePage()
{
    if (!m_updatePage)
    {
        m_updatePage = new UpdateCtrlWidget;
    }

    m_frameProxy->pushWidget(this, m_updatePage);
}
