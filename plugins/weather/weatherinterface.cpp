#include "weatherinterface.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>

void WeatherImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isArray())
    {
        QJsonArray array = value.toArray();
        QJsonObject obj = array.at(0).toObject();

        QJsonValue desc = obj.value("description");
        if(desc.isString())
            item.setDescription(desc.toString());

        QJsonValue icon = obj.value("icon");
        if(icon.isString())
            item.setIcon(icon.toString());

        QJsonValue id = obj.value("id");
        if(id.isDouble())
            item.setId(id.toDouble());

        QJsonValue main = obj.value("main");
        if(main.isString())
            item.setMain(main.toString());
    }
}

void TemperatureImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isObject())
    {
        QJsonObject obj = value.toObject();
        QJsonValue day = obj.value("day");
        if(day.isDouble())
            item.setDay(day.toDouble());

        QJsonValue min = obj.value("min");
        if(min.isDouble())
            item.setMin(min.toDouble());

        QJsonValue max = obj.value("max");
        if(max.isDouble())
            item.setMax(max.toDouble());

        QJsonValue eve = obj.value("eve");
        if(eve.isDouble())
            item.setEve(eve.toDouble());

        QJsonValue night = obj.value("night");
        if(night.isDouble())
            item.setNight(night.toDouble());

        QJsonValue morn = obj.value("morn");
        if(morn.isDouble())
            item.setMorn(morn.toDouble());
    }
}

void PressureImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isDouble())
        item.setPressure(value.toDouble());
}

void HumidityImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isDouble())
        item.setHumidity(value.toDouble());
}

void SpeedImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isDouble())
        item.setSpeed(value.toDouble());
}

void DegImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isDouble())
        item.setDeg(value.toDouble());
}

void CloudsImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isDouble())
        item.setClouds(value.toDouble());
}

void RainImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return ;

    if(value.isDouble())
        item.setRain(value.toDouble());
}

void DtImp::readData(WeatherItem &item, const QJsonValue &value)
{
    if(value.isUndefined())
        return;

    if(value.isDouble())
        item.setDt(value.toDouble());
}
