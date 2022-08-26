// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QLocale>
#include <QFile>
#include <QSettings>
#include <QDebug>

#include <crypt.h>
#include <random>

static const QStringList DCC_CONFIG_FILES {
    "/etc/deepin/dde-control-center.conf",
    "/usr/share/dde-control-center/dde-control-center.conf"
};

static const QMap<QString, QString> SYSTEM_LOCAL_MAP {
    {"zh_CN", "zh_CN"},
    {"zh_HK", "zh_HK"},
    {"zh_TW", "zh_TW"},
};

static const QStringList SYSTEM_LOCAL_LIST {
    "zh_CN",
    "zh_HK",
    "zh_TW",
    "ug_CN",    // 维语
    "bo_CN"     // 藏语
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
    QString lang = SYSTEM_LOCAL_LIST.contains(locale) ? locale : "en_US";

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

    return buf;
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

    return buf;
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

/**
 * @brief crypt函数是否支持SM3算法
 * crypt使用固定的key和salt进行加密，把加密结果与已知的正确结果进行比对，相等则支持，反之不支持
 * @return true 支持
 * @return false 不支持
 */
inline bool supportSM3()
{
    char password[] = "Hello world!";
    char salt[] = "$8$saltstring";
    const QString cryptResult = "$8$saltstring$6RCuSuWbADZmLALkvsvtcYYzhrw3xxpuDcqwdPIWxTD";
    return crypt(password, salt) == cryptResult;
}

inline QString cryptUserPassword(const QString &password)
{
    /*
        NOTE(kirigaya): Password is a combination of salt and crypt function.
        slat is begin with $6$, 16 byte of random values, at the end of $.
        crypt function will return encrypted values.
     */
    const QString seedChars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";
    if (supportSM3()) {
        qInfo() << "SM3 is supported";
        salt[1] = '8';
    }

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedChars.size() - 1); //seedChars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedChars.at(uniform_dist(e1)).toLatin1();
    }

    return crypt(password.toUtf8().data(), salt);
}

#endif // UTILS_H
