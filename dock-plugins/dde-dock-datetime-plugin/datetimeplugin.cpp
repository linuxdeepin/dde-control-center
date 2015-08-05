#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "datetimeplugin.h"
#include "clockpixmap.h"

static const QString MenuIdDatetimeSettings = "id_datetime_settings";
static const QString MenuIdSwitchDisplayMode = "id_switch_display_mode";
static const QString MenuIdShowWeek = "id_show_week:checkbox:show";
static const QString MenuIdShowDate = "id_show_date:checkbox:show";

DateTimePlugin::DateTimePlugin() :
    QObject(),
    m_showWeek(false),
    m_showDate(false)
{
    m_clockPixmap = ClockPixmap(QTime::currentTime());

    m_item = new QLabel;
    m_item->setStyleSheet("QLabel { color: white }");
    m_item->adjustSize();

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    m_timer->setSingleShot(false);
    m_timer->start();

    initCalendar();

    connect(m_timer, &QTimer::timeout, this, &DateTimePlugin::updateTime);
}

DateTimePlugin::~DateTimePlugin()
{

}

void DateTimePlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_proxy->itemAddedEvent(m_uuid);

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
    if (m_calendar)
        return m_calendar;
}

void DateTimePlugin::changeMode(Dock::DockMode newMode,
                                Dock::DockMode)
{
    setMode(newMode);
}

void DateTimePlugin::updateTime()
{
    QTime time = QTime::currentTime();
    QDate today = QDate::currentDate();

    if (m_mode == Dock::FashionMode) {
        m_clockPixmap.setTime(time);

        // if m_clockPixmap refuse to update its time,
        // there's no need to update our label for now;
        if (m_clockPixmap.getTime() == time) {
            m_item->setPixmap(m_clockPixmap);
        }
    } else {
        QString oldText = m_item->text();
        QString newText = "";

        if (m_showDate) {
            newText.append(today.toString("MMMdd"));
        }

        if (m_showWeek) {
            newText.append(today.toString("dddd"));
        }

        newText.append(time.toString(Qt::DefaultLocaleShortDate));

        m_item->setText(newText);

        if (newText.length() != oldText.length()) {
            m_item->adjustSize();
            m_proxy->itemSizeChangedEvent(m_uuid);
        }
    }
}

QString DateTimePlugin::getMenuContent(QString)
{
    QJsonObject contentObj;

    QJsonArray items;

    if (m_mode == Dock::FashionMode) {
        items.append(createMenuItem(MenuIdSwitchDisplayMode, "Switch display mode"));
    } else {
        items.append(createMenuItem(MenuIdShowWeek, "Show week", true, m_showWeek));
        items.append(createMenuItem(MenuIdShowDate, "Show date", true, m_showDate));
    }

    items.append(createMenuItem(MenuIdDatetimeSettings, "Datetime settings"));

    contentObj.insert("items", items);

    return QString(QJsonDocument(contentObj).toJson());
}

void DateTimePlugin::invokeMenuItem(QString, QString itemId, bool checked)
{
    if (itemId == MenuIdSwitchDisplayMode) {
        m_clockPixmap.setAnalog(!m_clockPixmap.getAnalog());
        m_item->setPixmap(m_clockPixmap);
    } else if (itemId == MenuIdShowWeek) {
        m_showWeek = checked;
    } else if (itemId == MenuIdShowDate) {
        m_showDate = checked;
    }
}

void DateTimePlugin::initCalendar()
{
    m_calendar = new DCalendar();
    m_calendar->setFixedSize(300, 300);
}

// private methods
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
    m_proxy->itemSizeChangedEvent(m_uuid);
}

QJsonObject DateTimePlugin::createMenuItem(QString itemId, QString itemName, bool checkable, bool checked)
{
    QJsonObject itemObj;

    itemObj.insert("itemId", itemId);
    itemObj.insert("itemText", itemName);
    itemObj.insert("itemIcon", "");
    itemObj.insert("itemIconHover", "");
    itemObj.insert("itemIconInactive", "");
    itemObj.insert("itemExtra", "");
    itemObj.insert("isActive", true);
    itemObj.insert("isCheckable", checkable);
    itemObj.insert("checked", checked);
    itemObj.insert("itemSubMenu", QJsonObject());

    return itemObj;
}
