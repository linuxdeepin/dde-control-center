#include "networkutil.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QSettings>
#include "GeoIP.h"
#include "GeoIPCity.h"

#ifdef   __cplusplus
extern "C" {
#endif

GEOIP_API unsigned long _GeoIP_lookupaddress(const char *host);

#ifdef   __cplusplus
}
#endif

const QString NetworkUtil::getHtml(const QString &url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseData = reply->readAll();
    reply->deleteLater();
    manager->deleteLater();
    return QString(responseData);
}

const QString NetworkUtil::getNetIP(QString code)
{
    if(code.isEmpty())
    {
        qDebug()<<"network invalid";
        return QString("0.0.0.0");
    }

    QString web = code.replace(" ", "");
    web = web.replace("\r", "");
    web = web.replace("\n", "");
    QStringList list = web.split("<br/>");
    if(list.size() < 4)
        return "0.0.0.0";
    QString tar = list[3];
    QStringList ip = tar.split("=");
    return (ip.count() > 1) ? ip[1] : QString("0.0.0.0");
    }

    const City NetworkUtil::ip2city(const QString &ip)
    {
    GeoIP * gi = GeoIP_open_type(GEOIP_CITY_EDITION_REV1, GEOIP_STANDARD | GEOIP_SILENCE);
    uint32_t ipnum = _GeoIP_lookupaddress(ip.toStdString().c_str());
    GeoIPRecord *gir = GeoIP_record_by_ipnum(gi, ipnum);

    if (gir) {
        const char *region = GeoIP_region_name_by_code(gir->country_code, gir->region);
        return City { "", gir->country_name, region, gir->city, "", gir->latitude, gir->longitude };
    }

    return City{};
}

const City NetworkUtil::city()
{
    QString ip = getNetIP(getHtml("http://whois.pconline.com.cn/"));
    City city = ip2city(ip);
    return city;
}
