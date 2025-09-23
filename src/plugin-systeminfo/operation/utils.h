// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UTILS_H
#define UTILS_H

#include <dsysinfo.h>

#include <QFile>
#include <QLocale>
#include <QSettings>

#define IS_SERVER_SYSTEM (DSysInfo::UosServer == DSysInfo::uosType())                    // 是否是服务器版
#define IS_COMMUNITY_SYSTEM (DSysInfo::UosCommunity == DSysInfo::uosEditionType())       // 是否是社区版
#define IS_PROFESSIONAL_SYSTEM (DSysInfo::UosProfessional == DSysInfo::uosEditionType()) // 是否是专业版
#define IS_HOME_SYSTEM (DSysInfo::UosHome == DSysInfo::uosEditionType())                 // 是否是个人版
#define IS_EDUCATION_SYSTEM (DSysInfo::UosEducation == DSysInfo::uosEditionType())       // 是否是教育版
#define IS_DEEPIN_DESKTOP (DSysInfo::DeepinDesktop == DSysInfo::deepinType())            // 是否是Deepin桌面

inline const static QString serverEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-CN-%1.%2";
inline const static QString serverEnduserAgreement_old =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/"
        "End-User-License-Agreement-Server-CN-%1.%2";
inline const static QString eulerServerEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-Euler-%1.%2";
inline const static QString homeEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Home-CN-%1.%2";
inline const static QString homeEnduserAgreement_old =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/"
        "End-User-License-Agreement-Home-CN-%1.%2";
inline const static QString militaryEnduserAgreement =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Military-%1.%2";
inline const static QString professionalEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Professional-CN-%1.%2";
inline const static QString professionalEnduserAgreement_old =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/"
        "End-User-License-Agreement-Professional-CN-%1.%2";
inline const static QString educationEnduserAgreement =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Education-CN-%1.%2";
inline const static QString oldAgreement =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.%2";
inline static const QStringList DCC_CONFIG_FILES{
    "/etc/deepin/dde-control-center.conf", "/usr/share/dde-control-center/dde-control-center.conf"
};

inline static const QMap<QString, QString> SYSTEM_LOCAL_MAP{
    { "zh_CN", "zh_CN" },
    { "zh_HK", "zh_HK" },
    { "zh_TW", "zh_TW" },
};

inline static const QStringList SYSTEM_LOCAL_LIST{
    "zh_CN",
    "zh_HK",
    "zh_TW",
    "ug_CN", // 维语
    "bo_CN"  // 藏语
};

inline bool isFileExist(const QString &path)
{
    QFile file(path);
    return file.exists();
}

namespace DCC_LICENSE {
using Dtk::Core::DSysInfo;

static const QString getLicensePath(const QString &filePath, const QString &type)
{
    const QString &locale{ QLocale::system().name() };
    QString lang = SYSTEM_LOCAL_LIST.contains(locale) ? locale : "en_US";

    QString path = QString(filePath).arg(lang).arg(type);
    if (isFileExist(path))
        return path;
    else
        return QString(filePath).arg("en_US").arg(type);
}

inline const QString getLicenseText(const QString &filePath, const QString &type)
{
    QFile license(getLicensePath(filePath, type));
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return buf;
}

[[maybe_unused]] inline const QString getDevelopModeLicense(const QString &filePath,
                                                            const QString &type)
{
    const QString &locale{ QLocale::system().name() };
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

[[maybe_unused]] inline void getPrivacyFile(QString &zhCNContent, QString &enUSContent)
{
    // 使用新的协议文档路径
    const QString newCNContent = "/usr/share/protocol/privacy-policy/Privacy-Policy-CN-zh_CN.md";
    const QString newENContent = "/usr/share/protocol/privacy-policy/Privacy-Policy-CN-en_US.md";

    const QString oldCNContent = "/usr/share/deepin-deepinid-client/privacy/deepinid-CN-zh_CN.md";
    const QString oldENContent = "/usr/share/deepin-deepinid-client/privacy/deepinid-CN-en_US.md";

    QFile privacyzhCNFile(newCNContent);
    zhCNContent = privacyzhCNFile.exists() ? newCNContent : oldENContent;
    QFile privacyenUSFile(newENContent);
    enUSContent = privacyzhCNFile.exists() ? newENContent : oldENContent;
    // 目前社区版的协议只放在这个路径下,后续如果修改了,再作适配
    if (DSysInfo::isCommunityEdition()) {
        zhCNContent = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy-Community/"
                      "Privacy-Policy-CN-zh_CN.md";
        enUSContent = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy-Community/"
                      "Privacy-Policy-CN-en_US.md";
    }
}

[[maybe_unused]] inline QString getUserExpContent()
{
    QString userExpContent = getLicensePath("/usr/share/protocol/userexperience-agreement/"
                                            "User-Experience-Program-License-Agreement-CN-%1.%2",
                                            "md");
    if (DSysInfo::isCommunityEdition()) {
        userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/"
                                        "User-Experience-Program-License-Agreement-Community/"
                                        "User-Experience-Program-License-Agreement-CN-%1.%2",
                                        "md");
        return userExpContent;
    }
    QFile newfile(userExpContent);
    if (false == newfile.exists()) {
        userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/"
                                        "User-Experience-Program-License-Agreement/"
                                        "User-Experience-Program-License-Agreement-CN-%1.%2",
                                        "md");
        QFile file(userExpContent);
        if (false == file.exists()) {
            userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/"
                                            "User-Experience-Program-License-Agreement-%1.%2",
                                            "md");
        }
    }
    return userExpContent;
}

struct LicenseSearchInfo
{
    bool exist;
    QString path;
};

inline LicenseSearchInfo isEndUserAgreementExist()
{
    auto pathIfExists = [](const QString &pattern, const QString &type) -> QString {
        const QString p = getLicensePath(pattern, type);
        return QFile::exists(p) ? p : QString();
    };

    QString candidate;

    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer) {
        candidate = pathIfExists(serverEnduserAgreement_new, "txt");
        if (candidate.isEmpty())
            candidate = pathIfExists(serverEnduserAgreement_old, "txt");
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosHome) {
        candidate = pathIfExists(homeEnduserAgreement_new, "txt");
        if (candidate.isEmpty())
            candidate = pathIfExists(homeEnduserAgreement_old, "txt");
    } else if (DSysInfo::isCommunityEdition()) {
        candidate = pathIfExists(
            "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Community/"
            "End-User-License-Agreement-CN-%1.%2",
            "txt");
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosEducation) {
        candidate = pathIfExists(educationEnduserAgreement, "txt");
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosMilitary) {
        candidate = pathIfExists(militaryEnduserAgreement, "txt");
    } else {
        candidate = pathIfExists(professionalEnduserAgreement_new, "txt");
        if (candidate.isEmpty())
            candidate = pathIfExists(professionalEnduserAgreement_old, "txt");
    }

    if (candidate.isEmpty())
        candidate = pathIfExists(oldAgreement, "txt");

    if (!candidate.isEmpty())
        return { true, candidate };

    return { false, QString() };
}

inline QString getEndUserAgreement(const QString &licensePath)
{
    QFile license(licensePath);
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return buf;
}

inline QString getEulerEndUserAgreement()
{
    const QString bodypath_new = getLicensePath(eulerServerEnduserAgreement_new, "txt");
    if (QFile::exists(bodypath_new)) {
        return getLicenseText(eulerServerEnduserAgreement_new, "txt");
    } else {
        return getLicenseText(
                "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.%2",
                "txt");
    }
}

inline QPair<QString, QString> loadLicenses()
{
    const QString title = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title");
    const QString body = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body");
    return QPair<QString, QString>(title, body);
}
} // namespace DCC_LICENSE

template<typename T>
T valueByQSettings(const QStringList &configFiles,
                   const QString &group,
                   const QString &key,
                   const QVariant &failback)
{
    for (const QString &path : configFiles) {
        QSettings settings(path, QSettings::IniFormat);
        if (!group.isEmpty()) {
            settings.beginGroup(group);
        }

        const QVariant &v = settings.value(key);
        if (v.isValid()) {
            T t = v.value<T>();
            return t;
        }
    }

    return failback.value<T>();
}

#endif // UTILS_H
