#include "weatherrequest.h"
#include "weatherinterface.h"
#include "networkutil.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include <QTimer>
#include <QProcess>

WeatherRequest::WeatherRequest(QObject *parent) :
    QObject(parent)
{
    m_maps["dt"] = new DtImp();
    m_maps["temp"] = new TemperatureImp();
    m_maps["weather"] = new WeatherImp();
    m_maps["pressure"] = new PressureImp();
    m_maps["humidity"] = new HumidityImp();
    m_maps["speed"] = new SpeedImp();
    m_maps["deg"] = new DegImp();
    m_maps["clouds"] = new CloudsImp();
    //m_maps["rain"] = new RainImp();
    m_loader = new LoaderCity(this);

    m_location = QUrl(QString("http://int.dpool.sina.com.cn/iplookup/iplookup.php?format=js"));

    m_manager = new QNetworkAccessManager(this);
    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(false);

    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this,
            SLOT(replyFinished(QNetworkReply*)));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    connect(m_loader, SIGNAL(city(const QString&)), this, SLOT(setCity(const QString&)));
//    m_url = QUrl(QString("http://www.ipip.net/ip.html"));
    m_loader->start();
}

WeatherRequest::~WeatherRequest()
{
    QList<WInterface*> interfaces = m_maps.values();
    qDeleteAll(interfaces.begin(), interfaces.end());
}

void WeatherRequest::setCity(const QString &city)
{
    m_city = city;
    m_url = QUrl(QString("http://api.openweathermap.org/data/2.5/forecast/daily?"
                 "q=%1&mode=json&units=metric&cnt=7&APPID=a106333152baafe953f41c112767b167").arg(city));

    m_manager->get(QNetworkRequest(m_url));
}

QString WeatherRequest::city() const
{
    return m_city;
}

int WeatherRequest::count() const
{
    return m_items.count();
}

WeatherItem WeatherRequest::dayAt(int index)
{
    if(index >=0 && index<m_items.count())
    {
        return m_items[index];
    }
    return WeatherItem();
}

void WeatherRequest::replyFinished(QNetworkReply *reply)
{
    m_items.clear();
    QByteArray ba = reply->readAll();
    reply->deleteLater();

    QJsonObject jobj = QJsonDocument::fromJson(ba).object();
    if(jobj.contains("list"))
    {
        QJsonValue value = jobj.take("list");
        if(value.isArray())
        {
            QJsonArray array = value.toArray();
            QJsonArray::iterator it = array.begin();
            for(;it != array.end(); it++)
            {
                if((*it).isObject())
                {
                    QJsonObject obj = (*it).toObject();
                    QStringList keys = m_maps.keys();
                    QStringList::iterator keyIt = keys.begin();
                    WeatherItem item;
                    for(; keyIt != keys.end(); keyIt++)
                    {
                        m_maps[*keyIt]->readData(item,obj.take(*keyIt));
                    }
                    m_items.append(item);
                }
            }
        }
    }
    emit refreshData(m_items);

    if(!m_timer->isActive())
    {
        m_timer->start();
    }
}

void WeatherRequest::slotTimeout()
{
    m_manager->get(QNetworkRequest(m_url));
}

LoaderCity::LoaderCity(QObject *parent)
    :QThread(parent)
{

}

void LoaderCity::run()
{
    NetworkUtil util;
    emit city(util.city());
}
