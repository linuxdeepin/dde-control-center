#include "personalizationmodule.h"
#include "personalizationwidget.h"
#include "personalizationmodel.h"
#include "personalizationwork.h"
#include "module/themewidget/themewidget.h"
#include "module/fontswidget/fontswidget.h"
#include "module/fontswidget/fontlistwidget.h"

#include <QElapsedTimer>

using namespace dcc;
using namespace dcc::personalization;

PersonalizationModule::PersonalizationModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_personalizationWidget(nullptr),
      m_model(nullptr),
      m_work(nullptr)
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
        m_personalizationWidget->setModel(m_model);
        connect(m_personalizationWidget, &PersonalizationWidget::showThemeWidget, this, &PersonalizationModule::showThemeWidget);
        connect(m_personalizationWidget, &PersonalizationWidget::showFontsWidget, this, &PersonalizationModule::showFontsWidget);
        connect(m_personalizationWidget, &PersonalizationWidget::requestSwitchWM, m_work, &PersonalizationWork::switchWM);
    }
    return m_personalizationWidget;
}

const QString PersonalizationModule::name() const
{
    return QStringLiteral("personalization");
}

void PersonalizationModule::showThemeWidget()
{
    ThemeWidget *themeWidget = new ThemeWidget;

    themeWidget->setModel(m_model);
    connect(themeWidget, &ThemeWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);

    m_frameProxy->pushWidget(this, themeWidget);
}

void PersonalizationModule::showFontsWidget()
{
    FontsWidget *fontsWidget = new FontsWidget;
    fontsWidget->setModel(m_model);
    connect(fontsWidget, &FontsWidget::showStandardFont, this, &PersonalizationModule::showStanardFontsListWidget);
    connect(fontsWidget, &FontsWidget::showMonoFont,    this, &PersonalizationModule::showMonoFontsListWidget);
    connect(fontsWidget, &FontsWidget::requestSetFontSize, m_work, &PersonalizationWork::setFontSize);

    m_frameProxy->pushWidget(this, fontsWidget);
}

void PersonalizationModule::showStanardFontsListWidget()
{
    FontListWidget *StandardFontList = new FontListWidget(tr("Standard Font"));
    StandardFontList->setModel(m_model->getStandFontModel());
    connect(StandardFontList, &FontListWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);

    m_frameProxy->pushWidget(this, StandardFontList);
}

void PersonalizationModule::showMonoFontsListWidget()
{
    FontListWidget *MonoFontList = new FontListWidget(tr("Monospaced Font"));
    MonoFontList->setModel(m_model->getMonoFontModel());
    connect(MonoFontList, &FontListWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);

    m_frameProxy->pushWidget(this, MonoFontList);
}

PersonalizationModule::~PersonalizationModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void PersonalizationModule::contentPopped(ContentWidget *const w)
{
    w->deleteLater();
}
