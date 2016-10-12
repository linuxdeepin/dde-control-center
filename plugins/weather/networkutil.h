#ifndef NETWORKUTIL_H
#define NETWORKUTIL_H

#include <QString>

class NetworkUtil
{
public:
    NetworkUtil();

    QString getHtml(const QString& url);
    QString getNetIP(QString code);
    QString ip2city(const QString& ip);
    QString city();
};

#endif // NETWORKUTIL_H
