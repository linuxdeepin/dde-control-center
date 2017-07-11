#ifndef WEATHERITEM_H
#define WEATHERITEM_H

#include <QObject>
#include <QDate>

class WeatherItem : public QObject
{
    Q_OBJECT
public:
    explicit WeatherItem(QObject *parent = 0);
    WeatherItem(const WeatherItem &item);
    ~WeatherItem();

    QString name() const;
    void setName(const QString &name);

    void setFahrenheitDegree(const bool fahrenheit);
    bool isFahrenheit() const;

    QPair<int, int> temperature() const;
    void setTemperature(const int &min, const int &max);
    void setTemperature(const QPair<int, int> &temperature);

    QDate date() const;
    void setDate(const QDate &date);

    QString dayName() const;

    WeatherItem& operator=(const WeatherItem &rhs);

    QString description() const;
    void setDescription(const QString &description);

private:
    QString m_name;
    QString m_description;
    QDate m_date;
    QPair<int,int> m_temperature;
    bool m_fahrenheit;
};



#endif // WEATHERITEM_H
