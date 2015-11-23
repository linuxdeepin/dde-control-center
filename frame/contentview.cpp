#include <QDebug>
#include <QPluginLoader>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>

#include "contentview.h"
#include "sidebar.h"
#include "constants.h"
#include "controlcenterproxy.h"

ContentView::ContentView(ControlCenterProxy *proxy, QWidget *parent)
    : QFrame(parent),
    m_controlCenterProxy(proxy)
{
    m_pluginLoader = new QPluginLoader(this);
#ifdef QT_DEBUG
    m_pluginLoader->setLoadHints(QLibrary::ResolveAllSymbolsHint);
#else
#endif
    m_pluginsManager = PluginsManager::getInstance(this);

    m_sideBar = new SideBar(parent);

    m_leftSeparator = new DSeparatorVertical;
    m_rightSeparator = new DSeparatorVertical;

    m_lastPluginWidgetContainer = new QWidget;
    m_lastPluginWidgetContainerLayout = new QHBoxLayout(m_lastPluginWidgetContainer);

    m_lastPluginWidgetContainerLayout->setMargin(0);
    m_lastPluginWidgetContainerLayout->setSpacing(0);

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_leftSeparator);
    m_layout->addWidget(m_sideBar);
    m_layout->addWidget(m_rightSeparator);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    if(m_hideInLeft) {
        m_rightSeparator->hide();
        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowLeft);
        m_layout->insertWidget(0, m_lastPluginWidgetContainer);
    } else {
        m_leftSeparator->hide();
        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowRight);
        m_layout->addWidget(m_lastPluginWidgetContainer);
    }

    QFrame::hide();
    m_opacityEffect = new QGraphicsOpacityEffect;
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);

    m_showAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_showAni->setDuration(DCC::FrameAnimationDuration);

    m_hideAni = new QPropertyAnimation(m_opacityEffect, "opacity");
    m_hideAni->setDuration(DCC::FrameAnimationDuration);

    connect(m_sideBar, &SideBar::moduleSelected, this, &ContentView::onModuleSelected);
    connect(m_hideAni, &QPropertyAnimation::finished, this, &QFrame::hide);
    connect(m_showAni, &QPropertyAnimation::finished, this, static_cast<void (ContentView::*)()>(&ContentView::update));

    setLayout(m_layout);
}

ContentView::~ContentView()
{
    m_pluginLoader->deleteLater();
    m_opacityEffect->deleteLater();
    m_showAni->deleteLater();
    m_hideAni->deleteLater();
}

void ContentView::switchToModule(ModuleMetaData module)
{
    qDebug() << "load plugin: " << module.path;

    unloadPlugin();

    // load new plugin
    m_pluginLoader->setFileName(module.path);
    m_sideBar->switchToModule(module.id);

    QObject *instance = m_pluginLoader->instance();
    ModuleInterface *interface = qobject_cast<ModuleInterface *>(instance);

    do {
        if (!interface)
            break;
        m_lastPluginInterface = interface;

        QWidget *content = interface->getContent();
        if (!content)
            break;
        m_lastPluginInterface->setProxy(m_controlCenterProxy);
        m_lastPluginWidget = content;
        m_lastPluginWidget->setFixedWidth(DCC::ModuleContentWidth);
        m_lastPluginWidgetContainerLayout->addWidget(m_lastPluginWidget);

        return;
    } while (false);

    // error
    qDebug() << m_pluginLoader->errorString();
}

void ContentView::hide()
{
    m_showAni->stop();
    m_hideAni->stop();
    m_hideAni->setStartValue(1.0);
    m_hideAni->setEndValue(0.0);
    m_hideAni->start();
}

void ContentView::show()
{
    m_hideAni->stop();
    m_showAni->stop();
    m_showAni->setStartValue(0.0);
    m_showAni->setEndValue(1.0);
    m_showAni->start();

    QFrame::show();
}

void ContentView::reLayout(bool hideInLeft)
{
    if(hideInLeft == m_hideInLeft)
        return;

    m_hideInLeft = hideInLeft;

    if(m_hideInLeft){
        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowLeft);
    }else{
        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowRight);
    }

    m_rightSeparator->setHidden(hideInLeft);
    m_leftSeparator->setVisible(hideInLeft);
    int index = hideInLeft ? 3 : 0;
    if(m_layout->count() > 3){
        QLayoutItem *item = m_layout->itemAt(index);
        QWidget *w = item->widget();
        if(w){
            m_layout->removeItem(item);
            m_layout->insertWidget(3 - index, w);
        }
    }
}

void ContentView::switchToModule(const QString pluginId)
{
    // unload old plugin
    m_pluginLoader->unload();
    // load new plugin
    m_pluginLoader->setFileName(m_pluginsManager->pluginPath(pluginId));
    m_sideBar->switchToModule(pluginId);

    QObject *instance = m_pluginLoader->instance();

    if (instance) {
        ModuleInterface *interface = qobject_cast<ModuleInterface *>(instance);
        if(m_hideInLeft)
            m_layout->insertWidget(0, interface->getContent());
        else
            m_layout->addWidget(interface->getContent());
    } else {
        qDebug() << m_pluginLoader->errorString();
    }
}

void ContentView::onModuleSelected(ModuleMetaData meta)
{
    qDebug() << meta.id;

    if (meta.id == "home")
    {
        // when goto home screen, notify plugin know.
        if (m_lastPluginInterface)
            m_lastPluginInterface->preUnload();

        emit backToHome();
        return;
    }

    if (meta.id == "shutdown")
    {
        emit shutdownSelected();
        return;
    }

    // switch to another plugin
    return switchToModule(meta);
}

void ContentView::unloadPlugin()
{
    if (m_lastPluginWidget)
    {
        m_lastPluginWidget->setParent(nullptr);
        m_lastPluginWidget->deleteLater();
        m_lastPluginWidget = nullptr;
    }

    if (m_lastPluginInterface)
    {
        m_lastPluginInterface->preUnload();
        delete m_lastPluginInterface;
        m_lastPluginInterface = nullptr;
    }
}
