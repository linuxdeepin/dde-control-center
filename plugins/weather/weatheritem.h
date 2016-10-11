#ifndef WEATHERITEM_H
#define WEATHERITEM_H

#include <QSharedDataPointer>
#include <QMap>

class WeatherItemData;

class WeatherItem
{
public:
    WeatherItem();
    WeatherItem(const WeatherItem &);
    WeatherItem &operator=(const WeatherItem &);
    ~WeatherItem();

    bool isValid() const;

    //dt
    void setDt(const double& dt);
    double dt() const;

    // temperature
    void setDay(const double& day);
    double day() const;
    void setMin(const double& min);
    double min() const;
    void setMax(const double& max);
    double max() const;
    void setNight(const double& night);
    double night() const;
    void setEve(const double& eve);
    double eve() const;
    void setMorn(const double& morn);
    double morn() const;

    void setPressure(const double& pre);
    double pressure() const;

    void setHumidity(const double& hum);
    double humidity() const;

    void setId(const double& id);
    double id() const;

    void setMain(const QString& main);
    QString main() const;

    void setDescription(const QString& descripe);
    QString description() const;

    void setIcon(const QString& icon);
    QString icon() const;

    void setSpeed(const double& speed);
    double speed() const;

    void setDeg(const double& deg);
    double deg() const;

    void setClouds(const double& clouds);
    double clouds() const;

    void setRain(const double& rain);
    double rain() const;

    QString dayOfWeek() const;

private:
    QSharedDataPointer<WeatherItemData> data;
    friend QDebug& operator<<(QDebug dbg, const WeatherItem& item);

};

QDebug& operator<<(QDebug dbg, const WeatherItem& item);

#endif // WEATHERITEM_H
