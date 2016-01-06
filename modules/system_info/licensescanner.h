#ifndef LICENSESSCANNER_H
#define LICENSESSCANNER_H

#include <QString>
#include <QMap>

class licenseScanner
{
public:
    static QMap<QString, QString> scan();
};

#endif // LICENSESSCANNER_H
