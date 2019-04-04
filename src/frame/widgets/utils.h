#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QLocale>
#include <QFile>
#include <QSettings>

static const QMap<QString, QString> SYSTEM_LOCAL_MAP {
    {"zh_CN", "zh_CN"},
#ifndef DCC_ENABLE_END_USER_LICENSE
    {"zh_TW", "zh_TW"},
#endif
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

template <typename T>
T valueByQSettings(const QString & file,
                   const QString & group,
                   const QString & key,
                   const QVariant &failback)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    T t = settings.value(key, failback).value<T>();
    settings.endGroup();

    return t;
}

#endif // UTILS_H
