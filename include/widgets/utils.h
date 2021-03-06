#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QLocale>
#include <QFile>
#include <QSettings>

static const QStringList DCC_CONFIG_FILES {
    "/etc/deepin/dde-control-center.conf",
    "/usr/share/dde-control-center/dde-control-center.conf"
};

static const QMap<QString, QString> SYSTEM_LOCAL_MAP {
    {"zh_CN", "zh_CN"},
    {"zh_HK", "zh_HK"},
    {"zh_TW", "zh_TW"},
};

enum UiActiveState {
    Unknown = -1,  //未知
    Unauthorized = 0,  //未授权
    Authorized,  //已授权
    AuthorizedLapse,  //授权失效
    TrialAuthorized, //试用期已授权
    TrialExpired //试用期已过期
};

inline bool isFileExist(const QString &path){
    QFile file(path);
    return file.exists();
}

static const QString getLicensePath(const QString &filePath, const QString &type)
{
    const QString& locale { QLocale::system().name() };
    QString lang = SYSTEM_LOCAL_MAP.keys().contains(locale) ? locale : "en_US";

    QString path = QString(filePath).arg(lang).arg(type);
    if (isFileExist(path))
        return path;
    else
        return QString(filePath).arg("en_US").arg(type);

}

inline const QString getLicenseText(const QString &filePath, const QString &type)
{
    QFile license(getLicensePath(filePath,type));
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return std::move(buf);
}

inline const QString getDevelopModeLicense(const QString &filePath, const QString &type)
{
    const QString& locale { QLocale::system().name() };
    QString lang;
    if (SYSTEM_LOCAL_MAP.keys().contains(locale)) {
        lang = { SYSTEM_LOCAL_MAP.value(QLocale::system().name(), "en_US") };
    }

    if (lang.isEmpty()) {
        lang = { SYSTEM_LOCAL_MAP.value(QLocale::system().name(), "en_US") };
    }

    QString path = QString(filePath).arg(lang).arg(type);
    QFile license(path);
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return std::move(buf);
}

template <typename T>
T valueByQSettings(const QStringList& configFiles,
                   const QString&     group,
                   const QString&     key,
                   const QVariant&    failback)
{
    for (const QString& path : configFiles) {
        QSettings settings(path, QSettings::IniFormat);
        if (!group.isEmpty()) {
            settings.beginGroup(group);
        }

        const QVariant& v = settings.value(key);
        if (v.isValid()) {
            T t = v.value<T>();
            return t;
        }
    }

    return failback.value<T>();
}

#endif // UTILS_H
