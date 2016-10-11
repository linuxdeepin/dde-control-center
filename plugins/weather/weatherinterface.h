#ifndef WEATHERINTERFACE_H
#define WEATHERINTERFACE_H

#include "weatheritem.h"
#include <QJsonValue>

class WInterface
{
public:
    virtual void readData(WeatherItem& item, const QJsonValue& value) = 0;
};

class DtImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class WeatherImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class TemperatureImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class PressureImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class HumidityImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class SpeedImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class DegImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class CloudsImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

class RainImp : public WInterface
{
public:
    void readData(WeatherItem &item, const QJsonValue &value);
};

#endif // WEATHERINTERFACE_H
