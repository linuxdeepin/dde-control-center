#include <QDebug>
#include <QPluginLoader>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>

#include "interfaces.h"
#include "contentview.h"
#include "sidebar.h"
#include "constants.h"

ContentView::ContentView(QList<ModuleMetaData> modules, bool hideInLeft, QWidget *parent)
    : QFrame(parent),
      m_hideInLeft(hideInLeft)
{
    m_pluginLoader = new QPluginLoader(this);

    m_sideBar = new SideBar(modules, parent);

    m_leftSeparator = new DSeparatorVertical;
    m_rightSeparator = new DSeparatorVertical;

    if(hideInLeft){
        m_rightSeparator->hide();
        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowLeft);
    }else{
        m_leftSeparator->hide();
        m_sideBar->getTipFrame()->setArrowDirection(DTipsFrame::ArrowRight);
    }

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_leftSeparator);
    m_layout->addWidget(m_sideBar);
    m_layout->addWidget(m_rightSeparator);
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

void ContentView::setModule(ModuleMetaData module)
{
    QElapsedTimer timer;

#ifdef QT_DEBUG
    timer.start();
#endif

    // unload old plugin
    m_pluginLoader->unload();

    qDebug() << "unload finished:" << timer.elapsed();

    m_pluginLoader->setFileName(module.path);
    m_sideBar->switchToModule(module);

    QObject *instance = m_pluginLoader->instance();

    qDebug() << "get instance finished:" << timer.elapsed();

    if (instance) {
        ModuleInterface *interface = qobject_cast<ModuleInterface *>(instance);
        if(m_hideInLeft)
            m_layout->insertWidget(0, interface->getContent());
        else
            m_layout->addWidget(interface->getContent());

        qDebug() << "loaded file name: " << m_pluginLoader->fileName();
    } else {
        qDebug() << m_pluginLoader->errorString();
    }

    qDebug() << "load module finished. The time spent:" << timer.elapsed();
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

void ContentView::onModuleSelected(ModuleMetaData meta)
{
    // switch to another plugin
    if (!meta.path.isEmpty()) {
        return setModule(meta);
    }

    // when goto home screen, notify plugin know.
    ModuleInterface *inter = qobject_cast<ModuleInterface *>(m_pluginLoader->instance());
    if (inter)
        inter->preUnload();

    emit homeSelected();

    if (meta.id == "shutdown") {
        emit shutdownSelected();
    }
}
