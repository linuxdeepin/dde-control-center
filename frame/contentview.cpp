#include <QDebug>
#include <QPluginLoader>
#include <QHBoxLayout>

#include "interfaces.h"
#include "contentview.h"
#include "sidebar.h"

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
}

void ContentView::setModule(ModuleMetaData module)
{
    qDebug() << "Loadind module " << module.name;

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
    }
}

void ContentView::onModuleSelected(ModuleMetaData meta)
{
    if (!meta.path.isNull() && !meta.path.isEmpty()) {
        setModule(meta);
    } else {
        emit homeSelected();
    }
}
