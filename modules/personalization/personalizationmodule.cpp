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
      m_personalizationWidget(nullptr),
      m_themeWidget(nullptr),
      m_fontSWidget(nullptr),
      m_StandardfontList(nullptr),
      m_MonofontList(nullptr)
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
    return QStringLiteral("personalization");
}

void PersonalizationModule::showThemeWidget()
{
    if (!m_themeWidget) {
        m_themeWidget = new ThemeWidget;
        m_themeWidget->setModel(m_model);
        connect(m_themeWidget, &ThemeWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);
    }
    m_frameProxy->pushWidget(this, m_themeWidget);
}

void PersonalizationModule::showFontsWidget()
{
    if (!m_fontSWidget) {
        m_fontSWidget = new FontsWidget;
        m_fontSWidget->setModel(m_model);
        connect(m_fontSWidget, &FontsWidget::showStandardFont, this, &PersonalizationModule::showStanardFontsListWidget);
        connect(m_fontSWidget, &FontsWidget::showMonoFont,    this, &PersonalizationModule::showMonoFontsListWidget);
        connect(m_fontSWidget, &FontsWidget::requestSetFontSize, m_work, &PersonalizationWork::setFontSize);
    }
    m_frameProxy->pushWidget(this, m_fontSWidget);
}

void PersonalizationModule::showStanardFontsListWidget()
{
    if (!m_StandardfontList) {
        m_StandardfontList = new FontListWidget(tr("Standard Font"));
        m_StandardfontList->setModel(m_model->getStandFontModel());
        connect(m_StandardfontList, &FontListWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);
    }
    m_frameProxy->pushWidget(this, m_StandardfontList);
}

void PersonalizationModule::showMonoFontsListWidget()
{
    if (!m_MonofontList) {
        m_MonofontList = new FontListWidget(tr("Monospaced Font"));
        m_MonofontList->setModel(m_model->getMonoFontModel());
        connect(m_MonofontList, &FontListWidget::requestSetDefault, m_work, &PersonalizationWork::setDefault);
    }
    m_frameProxy->pushWidget(this, m_MonofontList);
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
    if (w == m_StandardfontList)
        m_StandardfontList = nullptr;
    else if (w == m_MonofontList)
        m_MonofontList = nullptr;
    else if (w == m_themeWidget)
        m_themeWidget = nullptr;
    else if (w == m_fontSWidget)
        m_fontSWidget = nullptr;

    w->deleteLater();
}
