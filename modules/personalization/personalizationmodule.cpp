#include "personalizationmodule.h"
#include "personalizationwidget.h"
#include "personalizationmodel.h"
#include "personalizationwork.h"
#include "module/themewidget/themewidget.h"
#include "module/fontswidget/fontswidget.h"
#include "module/fontswidget/fontlistwidget.h"

using namespace dcc;
using namespace dcc::personalization;

PersonalizationModule::PersonalizationModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_personalizationWidget(nullptr)
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
    if (!m_personalizationWidget) {
        m_personalizationWidget = new PersonalizationWidget;
        connect(m_personalizationWidget, &PersonalizationWidget::showThemeWidget, this, &PersonalizationModule::showThemeWidget);
        connect(m_personalizationWidget, &PersonalizationWidget::showFontsWidget, this, &PersonalizationModule::showFontsWidget);
    }
    return m_personalizationWidget;
}

const QString PersonalizationModule::name() const
{
    return "Personalization";
}

void PersonalizationModule::showThemeWidget()
{
    ThemeWidget *w = new ThemeWidget;
    w->setModel(m_model);
    connect(w, &ThemeWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);
    m_frameProxy->pushWidget(this, w);
}

void PersonalizationModule::showFontsWidget()
{
    FontsWidget *w = new FontsWidget;
    w->setModel(m_model);

    connect(w, &FontsWidget::showStandardFont, this, &PersonalizationModule::showStanardFontsListWidget);
    connect(w, &FontsWidget::showMonoFont,    this, &PersonalizationModule::showMonoFontsListWidget);
    connect(w, &FontsWidget::requestSetFontSize, m_work, &PersonalizationWork::setFontSize);

    m_frameProxy->pushWidget(this, w);
}

void PersonalizationModule::showStanardFontsListWidget()
{
    FontListWidget *w = new FontListWidget(tr("Standard Font"));
    w->setModel(m_model->getStandFontModel());
    connect(w, &FontListWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);
    m_frameProxy->pushWidget(this, w);
}

void PersonalizationModule::showMonoFontsListWidget()
{
    FontListWidget *w = new FontListWidget(tr("Monospaced font"));
    w->setModel(m_model->getMonoFontModel());
    connect(w, &FontListWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);
    m_frameProxy->pushWidget(this, w);
}

PersonalizationModule::~PersonalizationModule()
{
    m_model->deleteLater();
    m_work->deleteLater();

    if (m_personalizationWidget) {
        m_personalizationWidget->deleteLater();
    }
}

void PersonalizationModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
