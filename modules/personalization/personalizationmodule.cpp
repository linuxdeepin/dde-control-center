#include "personalizationmodule.h"
using namespace dcc;
using namespace dcc::personalization;

PersonalizationModule::PersonalizationModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_PersonalizationWidget(nullptr)
{
}

void PersonalizationModule::initialize()
{
    m_model  = new PersonalizationModel;
    m_work = new PersonalizationWork(m_model);
    m_model->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

void PersonalizationModule::moduleActive()
{
    m_work->active();
}

void PersonalizationModule::moduleDeactive()
{
    m_work->deactive();
}

void PersonalizationModule::reset()
{

}

ModuleWidget *PersonalizationModule::moduleWidget()
{
    if (!m_PersonalizationWidget) {
        m_PersonalizationWidget = new PersonalizationWidget;
        connect(m_PersonalizationWidget, &PersonalizationWidget::showThemeWidget, this, &PersonalizationModule::showThemeWidget);
        connect(m_PersonalizationWidget, &PersonalizationWidget::showFontsWidget, this, &PersonalizationModule::showFontsWidget);
    }
    return m_PersonalizationWidget;
}

const QString PersonalizationModule::name() const
{
    return "Personalization";
}

void PersonalizationModule::showThemeWidget()
{
    ThemeWidget *w = new ThemeWidget;

    m_frameProxy->pushWidget(this, w);
}

void PersonalizationModule::showFontsWidget()
{
    FontsWidget *w = new FontsWidget;

    m_frameProxy->pushWidget(this, w);
}

PersonalizationModule::~PersonalizationModule()
{
    m_model->deleteLater();
    m_work->deleteLater();

    if (m_PersonalizationWidget) {
        m_PersonalizationWidget->deleteLater();
    }
}

void PersonalizationModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
