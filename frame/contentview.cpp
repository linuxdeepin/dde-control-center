#include <QDebug>
#include <QPluginLoader>
#include <QHBoxLayout>
#include <QPropertyAnimation>

#include "interfaces.h"
#include "contentview.h"
#include "sidebar.h"
#include "constants.h"

ContentView::ContentView(QList<ModuleMetaData> modules, QWidget *parent)
    : QFrame(parent)
{
    m_pluginLoader = NULL;

    m_sideBar = new SideBar(modules, this);

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_sideBar);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    connect(m_sideBar, &SideBar::moduleSelected, this, &ContentView::onModuleSelected);

    QFrame::hide();
    m_opacityEffect = new QGraphicsOpacityEffect;
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);
}

void ContentView::setModule(ModuleMetaData module)
{
    m_sideBar->switchToModule(module);

    if (m_pluginLoader) {
        if (m_pluginLoader->fileName() != module.path) {
            m_pluginLoader->unload();
        } else {
            return;
        }
    } else {
        qDebug() << "Loadind module " << module.name;
        m_pluginLoader = new QPluginLoader(this);
    }

    m_pluginLoader->setFileName(module.path);

    QObject * instance = m_pluginLoader->instance();
    if (instance) {
        ModuleInterface * interface = qobject_cast<ModuleInterface*>(instance);
        m_layout->addWidget(interface->getContent());
    }
}

void ContentView::hide()
{
    QPropertyAnimation *animation = new QPropertyAnimation(m_opacityEffect, "opacity");
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setDuration(DCC::FrameAnimationDuration);
    connect(animation, &QPropertyAnimation::finished, [this] () -> void {QFrame::hide();});
    animation->start();
}

void ContentView::show()
{
    QPropertyAnimation *animation = new QPropertyAnimation(m_opacityEffect, "opacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setDuration(DCC::FrameAnimationDuration);
    animation->start();
    QFrame::show();
}

void ContentView::onModuleSelected(ModuleMetaData meta)
{
    if (!meta.path.isNull() && !meta.path.isEmpty()) {
        setModule(meta);
    } else {
        emit homeSelected();
    }
}
