// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "protocolfile.h"
#include "../include/widgets/utils.h"

#include <DSysInfo>
#include <DConfig>

#include <QFile>

DCORE_USE_NAMESPACE


const static QString serverEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-CN-%1.txt";
const static QString serverEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/End-User-License-Agreement-Server-CN-%1.txt";
const static QString eulerServerEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-Euler-%1.txt";
const static QString eulerServerEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/End-User-License-Agreement-Server-Euler-%1.txt";
const static QString homeEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Home-CN-%1.txt";
const static QString homeEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/End-User-License-Agreement-Home-CN-%1.txt";
const static QString professionalEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Professional-CN-%1.txt";
const static QString professionalEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/End-User-License-Agreement-Professional-CN-%1.txt";
const static QString educationEnduserAgreement = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Education-CN-%1.txt";

void ProtocolFile::getPrivacyFile(QString &zhCN_Content, QString &enUS_Content)
{
    // 使用新的协议文档路径
    const QString zhCN_newContent = "/usr/share/protocol/privacy-policy/Privacy-Policy-CN-zh_CN.md";
    const QString enUS_newContent = "/usr/share/protocol/privacy-policy/Privacy-Policy-CN-en_US.md";

    const QString zhCN_oldContent = "/usr/share/deepin-deepinid-client/privacy/deepinid-CN-zh_CN.md";
    const QString enUS_oldContent = "/usr/share/deepin-deepinid-client/privacy/deepinid-CN-en_US.md";

    QFile privacyzhCNFile(zhCN_newContent);
    if (privacyzhCNFile.exists()) {
        zhCN_Content = zhCN_newContent;
    } else {
        zhCN_Content = zhCN_oldContent;
    }
    QFile privacyenUSFile(enUS_newContent);
    if (privacyenUSFile.exists()) {
        enUS_Content = enUS_newContent;
    } else {
        enUS_Content = enUS_oldContent;
    }

    //目前社区版的协议只放在这个路径下,后续如果修改了,再作适配
    if (DSysInfo::isCommunityEdition()) {
        zhCN_Content = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy-Community/Privacy-Policy-CN-zh_CN.md";
        enUS_Content = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy-Community/Privacy-Policy-CN-en_US.md";
    }
}

QString ProtocolFile::getUserExpContent()
{
    QString userExpContent = getLicensePath("/usr/share/protocol/userexperience-agreement/User-Experience-Program-License-Agreement-CN-%1.md", "");
    if (DSysInfo::isCommunityEdition()) {
        userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/User-Experience-Program-License-Agreement-Community/User-Experience-Program-License-Agreement-CN-%1.md", "");
        return userExpContent;
    }
    QFile newfile(userExpContent);
    if (false == newfile.exists()) {
        userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/User-Experience-Program-License-Agreement/User-Experience-Program-License-Agreement-CN-%1.md", "");
        QFile file(userExpContent);
        if (false == file.exists()) {
            userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/User-Experience-Program-License-Agreement-%1.md", "");
        }
    }
    return userExpContent;
}

QString ProtocolFile::getEnduserAgreement()
{
    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer) {
        const QString bodypath_new = getLicensePath(serverEnduserAgreement_new, "");
        if (QFile::exists(bodypath_new)) {
            const QString serverbody = getLicenseText(serverEnduserAgreement_new, "");
            return serverbody;
        }
        const QString bodypath_old = getLicensePath(serverEnduserAgreement_old, "");
        if (QFile::exists(bodypath_old)) {
            const QString serverbody = getLicenseText(serverEnduserAgreement_old, "");
            return serverbody;
        } else {
            const QString oldPody = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt", "");
            return oldPody;
        }
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosHome) {

        const QString bodypath_new = getLicensePath(homeEnduserAgreement_new, "");
        if (QFile::exists(bodypath_new)) {
            const QString serverbody = getLicenseText(homeEnduserAgreement_new, "");
            return serverbody;
        }
        const QString bodypath_old = getLicensePath(homeEnduserAgreement_old, "");
        if (QFile::exists(bodypath_old)) {
            const QString serverbody = getLicenseText(homeEnduserAgreement_old, "");
            return serverbody;
        } else {
            const QString oldPody = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt", "");
            return oldPody;
        }
    } else if (DSysInfo::isCommunityEdition()) {
        const QString body = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Community/End-User-License-Agreement-CN-%1.txt", "");
        return body;
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosEducation) {
        const QString bodypath = getLicensePath(educationEnduserAgreement, "");
        if (QFile::exists(bodypath)) {
            const QString educationbody = getLicenseText(educationEnduserAgreement, "");
            return educationbody;
        }
    } else {
        do {
            QObject raii;
            DConfig *config = DConfig::create("com.deepin.system.mil.dconfig", "com.deepin.system.mil.dconfig", QString(), &raii);
            if (!config) {
                qDebug() << "Can not find com.deepin.system.mil.dconfig or an error occurred in DTK";
                break;
            }
            if (!config->keyList().contains("Use_mil") || !config->keyList().contains("Mil_enduserAgreement_txt")) {
                qDebug() << "Mil enduser agreement was not found ";
                break;
            }
            const int &useMil = config->value("Use_mil", 1).toInt();
            const QString &agreementPath = config->value("Mil_enduserAgreement_txt", "").toString();
            if (useMil && !agreementPath.isEmpty() && QFile::exists(getLicensePath(agreementPath, ""))) {
                return getLicenseText(agreementPath, "");
            }
        } while (0);

        const QString bodypath_new = getLicensePath(professionalEnduserAgreement_new, "");
        if (QFile::exists(bodypath_new)) {
            const QString serverbody = getLicenseText(professionalEnduserAgreement_new, "");
            return serverbody;
        }
        const QString bodypath_old = getLicensePath(professionalEnduserAgreement_old, "");
        if (QFile::exists(bodypath_old)) {
            const QString serverbody = getLicenseText(professionalEnduserAgreement_old, "");
            return serverbody;
        } else {
            const QString oldPath = getLicenseText("/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt", "");
            return oldPath;
        }
    }
    return QString();
}

QString ProtocolFile::getEulerEnduserAgreement()
{
    QString serverBody = QString("");

    // 如果欧拉的最终用户许可协议文件获取不到，则使用服务器的最终用户许可协议
    if (QFile::exists(getLicensePath(eulerServerEnduserAgreement_new, ""))) {
        serverBody = getLicenseText(eulerServerEnduserAgreement_new, "");
    } else if (QFile::exists(getLicensePath(eulerServerEnduserAgreement_old, ""))) {
        serverBody = getLicenseText(eulerServerEnduserAgreement_old, "");
    } else if (QFile::exists(getLicensePath(serverEnduserAgreement_new, ""))) {
        serverBody = getLicenseText(serverEnduserAgreement_new, "");
    }

    return serverBody;
}
