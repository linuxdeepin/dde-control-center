#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QLocale>
#include <QFile>

static const QString getLicense(const QString &filePath, const QString &type)
{
    QString lang = QLocale::system().name();
    if (lang != "zh_CN")
        lang = "en_US";

    QString path = QString(filePath).arg(lang).arg(type);
    QFile license(path);
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return std::move(buf);
}

#endif // UTILS_H
