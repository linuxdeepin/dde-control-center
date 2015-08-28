#include <QDebug>
#include <QFrame>
#include <QDBusConnection>

#include "display.h"
#include "singlemonitorsettings.h"
#include "monitorground.h"
#include "monitor.h"
#include "displaymodeitem.h"

Display::Display():
    QObject(),
    m_frame(NULL)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_dbusDisplay = new DisplayInterface(this);

    initUI();
}

Display::~Display()
{
    qDebug() << "~Display()";
    delete m_frame ;
}

QFrame *Display::getContent()
{
    return m_frame;
}

void Display::initUI()
{
    m_frame = new QFrame;
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->setMargin(0);

    m_monitorGround = new MonitorGround;
    mainLayout->addWidget(m_monitorGround);

    foreach (const QDBusObjectPath &path, m_dbusDisplay->monitors()) {
        MonitorInterface *interface = new MonitorInterface(path.path(), this);
        m_dbusMonitors << interface;
        Monitor *monitor = new Monitor;
        monitor->setName(interface->name());
        m_monitors << monitor;
        m_monitorGround->addMonitor(monitor);
    }

    if(m_dbusMonitors.count() > 1){
        DArrowLineExpand *displayModeExpand = new DArrowLineExpand;
        displayModeExpand->setTitle(tr("Display Mode"));

        QWidget *widget = new QWidget;
        QHBoxLayout *layout = new QHBoxLayout;
        layout->setMargin(0);

        DisplayModeItem *item_copy = new DisplayModeItem;
        item_copy->setTitle(tr("Copy"));

        DisplayModeItem *item_extend = new DisplayModeItem;
        item_copy->setTitle(tr("Extend"));


        DisplayModeItem *item_settings = new DisplayModeItem;
        item_copy->setTitle(tr("Custom Settings"));

        layout->addWidget(item_copy);
        layout->addWidget(item_extend);
        layout->addWidget(item_settings);

        widget->setLayout(layout);

        displayModeExpand->setContent(widget);

        mainLayout->addWidget(displayModeExpand);
    }

    m_frame->setLayout(mainLayout);
}

void Display::updateUI()
{

}


