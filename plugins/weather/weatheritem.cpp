#include "weatheritem.h"

WeatherItem::WeatherItem(QObject *parent) :
    QObject(parent)
{

}

WeatherItem::WeatherItem(const WeatherItem &item) :
    QObject(),
    m_name(item.name()),
    m_date(item.date()),
    m_temperature(item.temperature()),
    m_fahrenheit(item.m_fahrenheit)
{

}

WeatherItem::~WeatherItem()
{

}

QString WeatherItem::name() const
{
    return m_name;
}

void WeatherItem::setName(const QString &name)
{
    m_name = name;
}

void WeatherItem::setFahrenheitDegree(const bool fahrenheit)
{
    m_fahrenheit = fahrenheit;
}

bool WeatherItem::isFahrenheit() const
{
    return m_fahrenheit;
}

QPair<int, int> WeatherItem::temperature() const
{
    return m_temperature;
}

void WeatherItem::setTemperature(const int &min, const int &max)
{
    setTemperature(QPair<int,int>(min, max));
}

void WeatherItem::setTemperature(const QPair<int, int> &temperature)
{
    m_temperature = temperature;
}

QDate WeatherItem::date() const
{
    return m_date;
}

void WeatherItem::setDate(const QDate &date)
{
    m_date = date;
}

QString WeatherItem::dayName() const
{
    return m_date.toString("ddd");
}

WeatherItem &WeatherItem::operator=(const WeatherItem &rhs)
{
    if (this != &rhs) {
        m_name = rhs.name();
        m_date = rhs.date();
        m_description = rhs.description();
        m_temperature = rhs.temperature();
        m_fahrenheit  = rhs.isFahrenheit();
    }

    return *this;
}

QString WeatherItem::description() const
{
    return m_description;
}

void WeatherItem::setDescription(const QString &description)
{
    m_description = description;
}
