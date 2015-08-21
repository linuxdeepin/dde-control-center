#include <QDebug>
#include <QPluginLoader>
#include <QHBoxLayout>
#include <QPropertyAnimation>

#include "interfaces.h"
#include "contentview.h"
#include "sidebar.h"
#include "constants.h"

#include <libdui/dseparatorvertical.h>

ContentView::ContentView(QList<ModuleMetaData> modules, QWidget *parent)
    : QFrame(parent)
{
    m_pluginLoader = NULL;

    m_sideBar = new SideBar(modules, parent);

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_sideBar);
    m_layout->addWidget(new DUI::DSeparatorVertical);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

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

    setLayout(m_layout);
}

ContentView::~ContentView()
{
    m_pluginLoader->deleteLater();
    m_opacityEffect->deleteLater();
    m_showAni->deleteLater();
    m_hideAni->deleteLater();
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
        m_pluginLoader = new QPluginLoader(this);
    }

    m_pluginLoader->setFileName(module.path);

    QObject * instance = m_pluginLoader->instance();
    if (instance) {
        ModuleInterface * interface = qobject_cast<ModuleInterface*>(instance);
        m_layout->addWidget(interface->getContent());

        qDebug() << "loaded file name: " << m_pluginLoader->fileName();
    } else {
        qDebug() << m_pluginLoader->errorString();
    }
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

void ContentView::onModuleSelected(ModuleMetaData meta)
{
    if (!meta.path.isNull() && !meta.path.isEmpty()) {
        setModule(meta);
    } else {
        emit homeSelected();
    }
}
