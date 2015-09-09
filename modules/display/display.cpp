#include <QDebug>
#include <QFrame>
#include <QDBusConnection>
#include <QMessageBox>

#include "moduleheader.h"
#include "listwidget.h"

#include "display.h"
#include "customsettings.h"
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

    connect(headerLine, &ModuleHeader::resetButtonClicked, m_dbusDisplay, &DisplayInterface::Reset);

    m_monitorGround = new MonitorGround(m_dbusDisplay);

    m_mainLayout->addWidget(headerLine);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(m_monitorGround);
    m_mainLayout->addWidget(new DSeparatorHorizontal);

    m_frame->setLayout(m_mainLayout);

    m_singleSettings = new CustomSettings(m_dbusDisplay, m_dbusMonitors);
    m_singleSettings->hide();

    updateUI();

    connect(m_dbusDisplay, &DisplayInterface::MonitorsChanged, this, &Display::updateUI, Qt::DirectConnection);
    connect(m_dbusDisplay, &DisplayInterface::DisplayModeChanged, this, &Display::onDisplayModeChanged, Qt::DirectConnection);
    connect(m_dbusDisplay, &DisplayInterface::PrimaryChanged, this, [=]{
        QString str = m_dbusDisplay->primary();

        for(int i=0; i<m_monitors.count(); ++i){
            m_monitors[i]->setIsPrimary(m_monitors[i]->name() == str);
        }
    }, Qt::DirectConnection);
}

void Display::updateUI()
{
    QList<QDBusObjectPath> pathList = m_dbusDisplay->monitors();

    m_monitorGround->clear();

    for (int i=0; i < m_monitors.count(); ++i) {
        m_monitors[i]->deleteLater();
        m_dbusMonitors[i]->deleteLater();
    }

    m_monitors.clear();
    m_dbusMonitors.clear();

    QDBusPendingReply<QString> reply_tmp = m_dbusDisplay->QueryCurrentPlanName();
    reply_tmp.waitForFinished();
    QStringList tmp_list = reply_tmp.value().split(",");

    QString primargName = m_dbusDisplay->primary();

    for (int i=0; i<pathList.count(); ++i) {
        MonitorInterface *interface = new MonitorInterface(pathList[i].path(), this);
        m_dbusMonitors << interface;
        Monitor *monitor = new Monitor(interface);

        if(tmp_list.count() == pathList.count())
            monitor->setName(interface->name());
        else if(i < tmp_list.count() - 1)
            monitor->setName(tmp_list[i]);

        if(monitor->name() == primargName)
            monitor->setIsPrimary(true);

        if(interface->isComposited()){
            Monitor *tmp = new Monitor(NULL);
            tmp->setName(tmp_list[1]);
            monitor->drop(tmp);
        }

        m_monitors << monitor;
        m_monitorGround->addMonitor(monitor);
    }

    m_singleSettings->updateUI(m_dbusMonitors);

    if(m_monitorNameList == tmp_list)
        return;

    m_monitorNameList = tmp_list;

    for(int i=4; i < m_mainLayout->count();){
        QLayoutItem *item = m_mainLayout->itemAt(i);
        QWidget *widget = item->widget();
        m_mainLayout->removeItem(item);
        delete item;

        if(widget == m_singleSettings){
            widget->hide();
        }else{
            widget->deleteLater();
        }
    }

    if(m_monitorNameList.count() > 1){
        DArrowLineExpand *displayModeExpand = new DArrowLineExpand;
        displayModeExpand->setTitle(tr("Display Mode"));

        m_widgetList = new ListWidget;
        m_widgetList->setItemSize(310, 90);
        m_widgetList->setCheckable(false);

        DisplayModeItem *item_copy = new DisplayModeItem;
        item_copy->setTitle(tr("Copy"));
        item_copy->setText(tr("Copy the contents of your primary screen to other screens."));
        item_copy->setIconName("copy");
        connect(item_copy, &DisplayModeItem::checkedChanged, this, [=](bool arg){
            if(arg && m_dbusDisplay->displayMode() != 1){
                qDebug()<<"SwitchMode(1";
                m_dbusDisplay->SwitchMode(1, "");
            }
        }, Qt::DirectConnection);

        DisplayModeItem *item_extend = new DisplayModeItem;
        item_extend->setTitle(tr("Extend"));
        item_extend->setText(tr("Extend your screen contents to display different contents on different screens."));
        item_extend->setIconName("extend");
        connect(item_extend, &DisplayModeItem::checkedChanged, this, [=](bool arg){
            if(arg && m_dbusDisplay->displayMode() != 2){
                qDebug()<<"SwitchMode(2";
                m_dbusDisplay->SwitchMode(2, "");
            }
        }, Qt::DirectConnection);

        DisplayModeItem *item_settings = new DisplayModeItem(false);
        item_settings->setTitle(tr("Custom Settings"));
        item_settings->setText(tr("You can do other custom settings to your screens."));
        item_settings->setIconName("customize");

        m_widgetList->addWidget(item_copy);
        m_widgetList->addWidget(item_extend);

        for (int i=0; i < m_monitorNameList.count(); ++i) {
            const QString& name = m_monitorNameList[i];

            DisplayModeItem *item_monitor = new DisplayModeItem;
            item_monitor->setTitle(tr("Only Displayed on %1").arg(name));
            item_monitor->setText(tr("Screen contents are only displayed on %1 but not on other screens.").arg(name));
            item_monitor->setIconName("single");
            item_monitor->setIconText(QString::number(i));
            m_widgetList->addWidget(item_monitor);

            connect(item_monitor, &DisplayModeItem::checkedChanged, this, [=](bool arg){
                if(arg && m_dbusDisplay->displayMode() != 3){
                    qDebug()<<"SwitchMode(3";
                    m_dbusDisplay->SwitchMode(3, name);
                }
            }, Qt::DirectConnection);
        }

        m_widgetList->addWidget(item_settings);

        displayModeExpand->setContent(m_widgetList);

        connect(item_settings, &DisplayModeItem::clicked, this, [this, displayModeExpand]{
            displayModeExpand->hide();
            m_singleSettings->show();
            m_monitorGround->beginEdit();
        }, Qt::DirectConnection);

        connect(m_singleSettings, &CustomSettings::cancel, this, [this, displayModeExpand]{
            displayModeExpand->show();
            m_singleSettings->hide();
            onDisplayModeChanged();
            m_monitorGround->endEdit();
        });

        m_mainLayout->addWidget(displayModeExpand);
        m_mainLayout->addWidget(m_singleSettings);

        onDisplayModeChanged();

        displayModeExpand->setExpand(true);
    }else{
        m_mainLayout->addWidget(m_singleSettings);
        m_singleSettings->show();
    }

    m_mainLayout->addStretch(1);
}

void Display::onDisplayModeChanged()
{
    switch (m_dbusDisplay->displayMode()) {
    case 0://custom mode
        m_widgetList->getWidget(m_monitorNameList.count() + 2)->setProperty("checked", true);
        break;
    case 1:{//copy mode
        m_widgetList->getWidget(0)->setProperty("checked", true);
        break;
    }
    case 2:{
        m_widgetList->getWidget(1)->setProperty("checked", true);
        break;
    }
    case 3:{
        QString str = m_dbusDisplay->primary();

        for(int i = 2; i<m_widgetList->count() - 1; ++i){
            DisplayModeItem *item = qobject_cast<DisplayModeItem*>(m_widgetList->getWidget(i));
            if(item && item->title().contains(str)){
                m_widgetList->getWidget(i)->setProperty("checked", true);
            }
        }
        break;
    }
    default:
        break;
    }
}


