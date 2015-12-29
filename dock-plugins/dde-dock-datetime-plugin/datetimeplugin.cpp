#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QCoreApplication>

#include "datetimeplugin.h"
#include "clockpixmap.h"
#include "dde-dock/dockconstants.h"

static const QString MenuIdDatetimeSettings = "id_datetime_settings";
static const QString MenuIdSwitchDisplayMode = "id_switch_display_mode";
static const QString MenuIdShowWeek = "id_show_week:checkbox:show";
static const QString MenuIdShowDate = "id_show_date:checkbox:show";

DateTimePlugin::DateTimePlugin() :
    QObject()
{
    initSettings();

    m_clockPixmap = ClockPixmap(QTime::currentTime());
    m_clockPixmap.setAnalog(clockAnalog());

    m_item = new QLabel;
    m_item->setAlignment(Qt::AlignLeft);
    m_item->setStyleSheet("QLabel { color: white }");
    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    m_timer->setSingleShot(false);
    m_timer->start();

    m_dateTime = new DBusTimedate(this);
    connect(m_dateTime, &DBusTimedate::Use24HourFormatChanged, this, &DateTimePlugin::onUse24HourFormatChanged);

    QTimer *initTimer = new QTimer(this);
    connect(initTimer, SIGNAL(timeout()), this, SLOT(onInitTimerTriggered()));
    initTimer->start(1000);

    connect(m_timer, &QTimer::timeout, this, &DateTimePlugin::updateTime);
}

DateTimePlugin::~DateTimePlugin()
{

}

void DateTimePlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    m_proxy->itemAddedEvent(m_id);

    setMode(proxy->dockMode());
}

QString DateTimePlugin::getPluginName()
{
    return "Datetime plugin";
}

QStringList DateTimePlugin::ids()
{
    return QStringList(m_id);
}

QString DateTimePlugin::getName(QString)
{
    return getPluginName();
}

QString DateTimePlugin::getTitle(QString)
{
    return getPluginName();
}

QString DateTimePlugin::getCommand(QString)
{
    return "dde-control-center datetime";
}

bool DateTimePlugin::configurable(const QString &)
{
    return false;
}

bool DateTimePlugin::enabled(const QString &)
{
    return true;
}

void DateTimePlugin::setEnabled(const QString &, bool)
{

}

QWidget * DateTimePlugin::getItem(QString)
{
    updateTime();
    return m_item;
}

QWidget * DateTimePlugin::getApplet(QString)
{
    if (m_calendar) {
        m_calendar->setCurrentDate(QDate::currentDate());
        return m_calendar;
    }
    else
        return NULL;
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

        //change date and time format in translation
        QString dateFormat = tr("MMM dd");
        QString timeFormat = m_use24HourFormat ? tr(" hh:mm ") : tr(" hh:mm A");
        QString weekFormat = tr("ddd");
        QString dateAndWeekFormat = tr("ddd MMM dd");

        if (showDate() && showWeek())
            newText += today.toString(dateAndWeekFormat);
        else if (showWeek())
            newText += today.toString(weekFormat);
        else if (showDate())
            newText += today.toString(dateFormat);
        newText += time.toString(timeFormat);

        m_item->setText(newText);

        if (newText.length() != oldText.length()) {
            QFontMetrics metrics(m_item->font());
            int textWidth = metrics.width(newText);
            int textHeight = metrics.height();
            m_item->setFixedSize(textWidth + RIGHT_PADDING, textHeight);
            m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeItemSize, m_id);
        }
    }
}

void DateTimePlugin::onUse24HourFormatChanged()
{
    m_use24HourFormat = m_dateTime->use24HourFormat();
    //force update pixmap for fashion mode
    m_clockPixmap.setIn24hour(m_use24HourFormat);
    if (m_mode == Dock::FashionMode)
        m_item->setPixmap(m_clockPixmap);
}

bool DateTimePlugin::showWeek()
{
    return m_settings->value("showWeek").toBool();
}

bool DateTimePlugin::showDate()
{
    return m_settings->value("showDate").toBool();
}

bool DateTimePlugin::clockAnalog()
{
    return m_settings->value("clockAnalog").toBool();
}

QString DateTimePlugin::getMenuContent(QString)
{
    QJsonObject contentObj;

    QJsonArray items;

    if (m_mode == Dock::FashionMode) {
        items.append(createMenuItem(MenuIdSwitchDisplayMode, tr("Switch display mode")));
    } else {
        items.append(createMenuItem(MenuIdShowWeek, tr("Show week"), true, showWeek()));
        items.append(createMenuItem(MenuIdShowDate, tr("Show date"), true, showDate()));
    }

    items.append(createMenuItem(MenuIdDatetimeSettings, tr("Datetime settings(_T)")));

    contentObj.insert("items", items);

    return QString(QJsonDocument(contentObj).toJson());
}

void DateTimePlugin::invokeMenuItem(QString id, QString itemId, bool checked)
{
    if (itemId == MenuIdSwitchDisplayMode) {
        m_settings->setValue("clockAnalog", !clockAnalog());
        m_clockPixmap.setAnalog(clockAnalog());
        m_item->setPixmap(m_clockPixmap);
    } else if (itemId == MenuIdShowWeek) {
        m_settings->setValue("showWeek", checked);
    } else if (itemId == MenuIdShowDate) {
        m_settings->setValue("showDate", checked);
    } else if (itemId == MenuIdDatetimeSettings) {
        QProcess::startDetached(getCommand(id));
    }
}

void DateTimePlugin::onInitTimerTriggered()
{
    QTimer *t = qobject_cast<QTimer *>(sender());

    if (t && m_dateTime->isValid()) {
        qWarning() << "DataTimePlugin: DBus data is ready!";
        t->stop();
        t->deleteLater();

        initCalendar();
        //for initialization
        onUse24HourFormatChanged();
    }
}

void DateTimePlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-datetime-plugin", this);

    if (!QFile::exists(m_settings->fileName())) {
        m_settings->setValue("showWeek", false);
        m_settings->setValue("showDate", false);
        m_settings->setValue("clockAnalog", false);
    }
}

void DateTimePlugin::initCalendar()
{
    m_calendar = new DCalendar();
	// reuse the translation of dde-control-center datetime module
    m_calendar->setSolarDisplayFormat(QCoreApplication::translate("NormalWidget", "dddd, dd MMMM yyyy"));
    m_calendar->setLunarVisible(QLocale::system().name().startsWith("zh_"));
    m_calendar->setLunarFestivalHighlight(false);
    m_calendar->setControlPanelVisible(false);
    m_calendar->setDateInfoVisible(true);
    m_calendar->setSelectionMode(DCalendar::NoSelection);
    m_calendar->setFixedSize(300, 300);
    m_calendar->updateCurrentDate();
}

// private methods
void DateTimePlugin::setMode(Dock::DockMode mode)
{
    m_mode = mode;

    QTime time = QTime::currentTime();

    if (m_mode == Dock::FashionMode) {
        m_item->setFixedSize(Dock::APPLET_FASHION_ITEM_WIDTH, Dock::APPLET_FASHION_ITEM_HEIGHT);
        m_item->setPixmap(m_clockPixmap);
    } else {
        QString timeFormat = m_use24HourFormat ? " hh:mm" : tr(" hh:mm A");
        m_item->setText(time.toString(timeFormat));

        QFontMetrics metrics(m_item->font());
        int textWidth = metrics.width(m_item->text());
        int textHeight = metrics.height();
        m_item->setFixedSize(textWidth + RIGHT_PADDING, textHeight);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeItemSize, m_id);
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
