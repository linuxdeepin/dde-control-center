/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangwei <wangwei@uniontech.com>
 *
 * Maintainer: wangwei <wangwei@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "protocolfile.h"
#include "../include/widgets/utils.h"

#include <DSysInfo>

#include <QFile>

DCORE_USE_NAMESPACE


const static QString serverEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-CN-%1.txt";
const static QString serverEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/End-User-License-Agreement-Server-CN-%1.txt";
const static QString homeEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Home-CN-%1.txt";
const static QString homeEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/End-User-License-Agreement-Home-CN-%1.txt";
const static QString professionalEnduserAgreement_new = "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Professional-CN-%1.txt";
const static QString professionalEnduserAgreement_old = "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/End-User-License-Agreement-Professional-CN-%1.txt";

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
}

QString ProtocolFile::getUserExpContent()
{
    QString userExpContent = getLicensePath("/usr/share/protocol/userexperience-agreement/User-Experience-Program-License-Agreement-CN-%1.md", "");
    if (DSysInfo::DeepinDesktop == DSysInfo::deepinType()) {
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
    } else {
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
}
