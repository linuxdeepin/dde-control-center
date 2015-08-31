#include <QDebug>
#include <QFrame>
#include <QDBusConnection>
#include <QMessageBox>

#include "moduleheader.h"

#include "display.h"
#include "singlemonitorsettings.h"
#include "monitorground.h"
#include "monitor.h"
#include "displaymodeitem.h"

Display::Display():
    QObject(),
    m_frame(NULL),
    m_mainLayout(new QVBoxLayout)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    init();
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

void Display::init()
{
    m_dbusDisplay = new DisplayInterface(this);

    m_frame = new QFrame;
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    ModuleHeader * headerLine = new ModuleHeader("Display");

    m_monitorGround = new MonitorGround;

    m_mainLayout->addWidget(headerLine);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(m_monitorGround);
    m_mainLayout->addWidget(new DSeparatorHorizontal);

    m_frame->setLayout(m_mainLayout);

    updateUI();

    connect(m_dbusDisplay, &DisplayInterface::MonitorsChanged, this, &Display::updateUI);
}

void Display::updateUI()
{
    QList<QDBusObjectPath> pathList = m_dbusDisplay->monitors();

    if(pathList.count() == m_monitors.count())
        return;

    m_monitorGround->clear();

    for (int i=0; i < m_monitors.count(); ++i) {
        m_monitors[i]->deleteLater();
        m_dbusMonitors[i]->deleteLater();
    }
    m_monitors.clear();
    m_dbusMonitors.clear();

    for(int i=4; i < m_mainLayout->count();){
        QLayoutItem *item = m_mainLayout->itemAt(i);
        QWidget *widget = item->widget();
        if(widget)
            widget->deleteLater();
        m_mainLayout->removeItem(item);
        delete item;
    }

    foreach (const QDBusObjectPath &path, pathList) {
        MonitorInterface *interface = new MonitorInterface(path.path(), this);
        m_dbusMonitors << interface;
        Monitor *monitor = new Monitor(interface);
        monitor->setName(interface->name());
        m_monitors << monitor;
        m_monitorGround->addMonitor(monitor);
    }

    if(m_dbusMonitors.count() > 1){
        DArrowLineExpand *displayModeExpand = new DArrowLineExpand;
        displayModeExpand->setTitle(tr("Display Mode"));

        QWidget *widget = new QWidget;
        widget->setFixedWidth(310);
        widget->setMinimumHeight(380);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->setMargin(0);
        layout->addSpacing(10);

        DisplayModeItem *item_copy = new DisplayModeItem;
        item_copy->setTitle(tr("Copy"));
        item_copy->setText(tr("Copy the contents of your primary screen to other screens."));
        item_copy->setIconName("copy");

        DisplayModeItem *item_extend = new DisplayModeItem;
        item_extend->setTitle(tr("Extend"));
        item_extend->setText(tr("Extend your screen contents to display different contents on different screens."));
        item_extend->setIconName("extend");

        DisplayModeItem *item_settings = new DisplayModeItem(false);
        item_settings->setTitle(tr("Custom Settings"));
        item_settings->setText(tr("You can do other custom settings to your screens."));
        item_settings->setIconName("customize");

        layout->addWidget(item_copy);
        layout->addWidget(item_extend);

        foreach (MonitorInterface *dbus, m_dbusMonitors) {
            DisplayModeItem *item_monitor = new DisplayModeItem;
            item_monitor->setTitle(tr("Only Displayed on %1").arg(dbus->name()));
            item_monitor->setText(tr("Screen contents are only displayed on %1 but not on other screens.").arg(dbus->name()));
            item_monitor->setIconName("single");
            layout->addWidget(item_monitor);
        }

        layout->addWidget(item_settings);
        layout->addStretch(1);
        widget->setLayout(layout);

        displayModeExpand->setContent(widget);

        SingleMonitorSettings *singleSettings = new SingleMonitorSettings(m_dbusDisplay, m_dbusMonitors);
        singleSettings->hide();

        m_mainLayout->addWidget(displayModeExpand);
        m_mainLayout->addWidget(singleSettings);

        connect(item_settings, &DisplayModeItem::activeChanged, [=](bool arg){
            if(arg){
                displayModeExpand->hide();
                singleSettings->show();
            }
        });
    }else{
        m_mainLayout->addWidget(new SingleMonitorSettings(m_dbusDisplay, m_dbusMonitors));
    }

    m_mainLayout->addStretch(1);
}


