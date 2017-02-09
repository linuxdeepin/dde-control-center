#ifndef NETWORKUTIL_H
#define NETWORKUTIL_H

#include <QString>

#include "types.h"

class NetworkUtil
{
public:
    static const QString getHtml(const QString& url);
    static const QString getNetIP(QString code);
    static const City ip2city(const QString& ip);
    static const City city();
};

#endif // NETWORKUTIL_H
