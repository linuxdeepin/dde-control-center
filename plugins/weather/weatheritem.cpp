#include "weatheritem.h"
#include <QDebug>
#include <QPainter>
#include <QDateTime>
#include <QMap>
/*
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm with light rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm with rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm with heavy rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["light thunderstorm"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy thunderstorm"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["ragged thunderstorm"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm with light drizzle"] ="";
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm with drizzle"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["thunderstorm with heavy drizzle"] = "";

QMap<QString, QString> WeatherItem::m_iconsDict["light intensity drizzle"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["drizzle"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy intensity drizzle"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["light intensity drizzle rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["drizzle rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy intensity drizzle rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["shower rain and drizzle"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy shower rain and drizzle"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["shower drizzle"] = "";

QMap<QString, QString> WeatherItem::m_iconsDict["light rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["moderate rain"] = ":moderate rain";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy intensity rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["very heavy rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["extreme rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["freezing rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["light intensity shower rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["shower rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy intensity shower rain"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["ragged shower rain"] = "";

QMap<QString, QString> WeatherItem::m_iconsDict["light snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["sleet"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["shower sleet"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["light rain and snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["rain and snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["light shower snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["shower snow"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["heavy shower snow"] = "";

QMap<QString, QString> WeatherItem::m_iconsDict["clear sky"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["few clouds"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["scattered clouds"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["broken clouds"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["overcast clouds"] = "";

QMap<QString, QString> WeatherItem::m_iconsDict["tornado"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["tropical storm"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["hurricane"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["cold"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["hot"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["windy"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["hail"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["fresh breeze"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["strong breeze"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["high wind, near gale"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["gale"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["severe gale"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["storm"] = "";
QMap<QString, QString> WeatherItem::m_iconsDict["violent storm"] = "";
*/

class WeatherItemData : public QSharedData
{
public:
    // dt
    double dt;
    //temperature
    double day;
    double min;
    double max;
    double night;
    double eve;
    double morn;

    double pressure;
    double humidity;

    //weather
    double id;
    QString main;
    QString description;
    QString icon;

    double speed;
    double deg;
    double clouds;
    double rain;
};

WeatherItem::WeatherItem() : data(new WeatherItemData)
{
    data->id = -1;
}

WeatherItem::WeatherItem(const WeatherItem &rhs) : data(rhs.data)
{

}

WeatherItem &WeatherItem::operator=(const WeatherItem &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

WeatherItem::~WeatherItem()
{

}

bool WeatherItem::isValid() const
{
    return (data->id != -1);
}

void WeatherItem::setDt(const double &dt)
{
    data->dt = dt;
}

double WeatherItem::dt() const
{
    return data->dt;
}

void WeatherItem::setDay(const double &day)
{
    data->day = day;
}

double WeatherItem::day() const
{
    return data->day;
}

void WeatherItem::setMin(const double &min)
{
    data->min = min;
}

double WeatherItem::min() const
{
    return data->min;
}

void WeatherItem::setMax(const double &max)
{
    data->max = max;
}

double WeatherItem::max() const
{
    return data->max;
}

void WeatherItem::setNight(const double &night)
{
    data->night = night;
}

double WeatherItem::night() const
{
    return data->night;
}

void WeatherItem::setEve(const double &eve)
{
    data->eve = eve;
}

double WeatherItem::eve() const
{
    return data->eve;
}

void WeatherItem::setMorn(const double &morn)
{
    data->morn = morn;
}

double WeatherItem::morn() const
{
    return data->morn;
}

void WeatherItem::setPressure(const double &pre)
{
    data->pressure = pre;
}

double WeatherItem::pressure() const
{
    return data->pressure;
}

void WeatherItem::setHumidity(const double &hum)
{
    data->humidity = hum;
}

double WeatherItem::humidity() const
{
    return data->humidity;
}

void WeatherItem::setId(const double &id)
{
    data->id = id;
}

double WeatherItem::id() const
{
    return data->id;
}

void WeatherItem::setMain(const QString &main)
{
    data->main = main;
}

QString WeatherItem::main() const
{
    return data->main;
}

void WeatherItem::setDescription(const QString &descripe)
{
    data->description = descripe;
}

QString WeatherItem::description() const
{
    return data->description;
}

void WeatherItem::setIcon(const QString &icon)
{
    data->icon = icon;
}

QString WeatherItem::icon() const
{
    return data->icon;
}

void WeatherItem::setSpeed(const double &speed)
{
    data->speed = speed;
}

double WeatherItem::speed() const
{
    return data->speed;
}

void WeatherItem::setDeg(const double &deg)
{
    data->deg = deg;
}

double WeatherItem::deg() const
{
    return data->deg;
}

void WeatherItem::setClouds(const double &clouds)
{
    data->clouds = clouds;
}

double WeatherItem::clouds() const
{
    return data->clouds;
}

void WeatherItem::setRain(const double &rain)
{
    data->rain = rain;
}

double WeatherItem::rain() const
{
    return data->rain;
}

QString WeatherItem::dayOfWeek() const
{
    if(!isValid())
        return QString();

    QDate date = QDateTime::fromTime_t(data->dt).date();
    return date.longDayName(date.dayOfWeek(), QDate::StandaloneFormat);
}

QDebug &operator<<(QDebug dbg, const WeatherItem &item)
{
    dbg.nospace()<<"WeatherItem ( dt: "<<item.dt()<<" , ";
    dbg.nospace()<<"day: "<<item.day()<<" , ";
    dbg.nospace()<<"min: "<<item.min()<<" , ";
    dbg.nospace()<<"max: "<<item.max()<<" , ";
    dbg.nospace()<<"night: "<<item.night()<<" , ";
    dbg.nospace()<<"eve: "<<item.eve()<<" , ";
    dbg.nospace()<<"morn: "<<item.morn()<<" , ";
    dbg.nospace()<<"pressure: "<<item.pressure()<<" , ";
    dbg.nospace()<<"humidity: "<<item.humidity()<<" , ";
    dbg.nospace()<<"id: "<<item.id()<<" , ";
    dbg.nospace()<<"main: "<<item.main()<<" , ";
    dbg.nospace()<<"description: "<<item.description()<<" , ";
    dbg.nospace()<<"icon: "<<item.icon()<<" , ";
    dbg.nospace()<<"speed: "<<item.speed()<<" , ";
    dbg.nospace()<<"deg: "<<item.deg()<<" , ";
    dbg.nospace()<<"clouds: "<<item.clouds()<<" , ";
    dbg.nospace()<<"rain: "<<item.rain()<<" )";
    return dbg.maybeSpace();
}
