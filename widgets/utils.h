#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QLocale>
#include <QFile>

static const QMap<QString, QString> SYSTEM_LOCAL_MAP {
    {"zh_CN", "zh_CN"},
    {"zh_TW", "zh_TW"},
};

static const QString getLicense(const QString &filePath, const QString &type)
{
    const QString lang { SYSTEM_LOCAL_MAP.value(QLocale::system().name(), "en_US") };

    QString path = QString(filePath).arg(lang).arg(type);
    QFile license(path);
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return std::move(buf);
}

#endif // UTILS_H
