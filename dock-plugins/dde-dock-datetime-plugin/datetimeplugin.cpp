#include <QLabel>
#include <QTimer>
#include <QDebug>

#include "datetimeplugin.h"
#include "clockpixmap.h"

DateTimePlugin::DateTimePlugin() :
    QObject()
{
    m_clockPixmap = ClockPixmap(QTime::currentTime());

    m_item = new QLabel;
    m_item->setStyleSheet("QLabel { color: white }");

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    m_timer->setSingleShot(false);
    m_timer->start();

    connect(m_timer, &QTimer::timeout, this, &DateTimePlugin::updateTime);
}

DateTimePlugin::~DateTimePlugin()
{
    m_item->deleteLater();
}

void DateTimePlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    setMode(proxy->dockMode());
}

QString DateTimePlugin::name()
{
    return "Datetime plugin";
}

QStringList DateTimePlugin::uuids()
{
    return QStringList(m_uuid);
}

QString DateTimePlugin::getTitle(QString)
{
    return name();
}

QWidget * DateTimePlugin::getItem(QString)
{
    return m_item;
}

QWidget * DateTimePlugin::getApplet(QString)
{
    return NULL;
}

void DateTimePlugin::changeMode(Dock::DockMode newMode,
                                Dock::DockMode)
{
    setMode(newMode);
}

void DateTimePlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    QTime time = QTime::currentTime();

    if (m_mode == Dock::FashionMode) {
        m_item->setPixmap(m_clockPixmap);
    } else {
        m_item->setText(time.toString(Qt::DefaultLocaleShortDate));
    }

    m_item->adjustSize();
    m_proxy->itemAddedEvent(m_uuid);
    m_proxy->itemSizeChangedEvent(m_uuid);
}

void DateTimePlugin::updateTime()
{
    QTime time = QTime::currentTime();

    if (m_mode == Dock::FashionMode) {
        m_clockPixmap.setTime(time);

        // if m_clockPixmap refuse to update its time,
        // there's no need to update our label for now;
        if (m_clockPixmap.getTime() == time) {
            m_item->setPixmap(m_clockPixmap);
        }
    } else {
        m_item->setText(time.toString(Qt::DefaultLocaleShortDate));
    }
}
