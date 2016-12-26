#ifndef NETWORKUTIL_H
#define NETWORKUTIL_H

#include <QString>

class NetworkUtil
{
public:
    NetworkUtil();

    static const QString getHtml(const QString& url);
    static const QString getNetIP(QString code);
    static const QString ip2city(const QString& ip);
    static const QString city();
};

#endif // NETWORKUTIL_H
