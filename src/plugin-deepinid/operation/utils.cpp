// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"
#include "hardwareinfo.h"

#include <DGuiApplicationHelper>
#include <DNotifySender>
#include <DSysInfo>
#include <DUtil>

#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QJsonObject>
#include <QLocale>
#include <QProcess>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

DCORE_USE_NAMESPACE

namespace utils {

QString getUrlTitle()
{
    QString url;
    if (qEnvironmentVariableIsEmpty("DEEPIN_PRE")) {
        url = IsCommunitySystem ? QStringLiteral("https://login.deepin.org") : QStringLiteral("https://login.uniontech.com");
    } else {
        url = IsCommunitySystem ? QStringLiteral("https://login-pre.deepin.org") : QStringLiteral("https://login-pre.uniontech.com");
    }
    return url;
}

QString forgetPwdURL()
{
    static QString forgetUrl;
    if (forgetUrl.isEmpty()) {
        forgetUrl = getUrlTitle() + QStringLiteral("/view/client/forgot-password");
    }

    QString templateURL = "%1";
    templateURL += "?lang=%2";
    templateURL += "&theme=%3";
    templateURL += "&color=%4";
    templateURL += "&font_family=%5";
    templateURL += "&client_version=%6";
    templateURL += "&device_kernel=%7";
    templateURL += "&device_processor=%8";
    templateURL += "&os_version=%9";
    templateURL += "&device_code=%10";
    templateURL += "&user_name=%11";
    templateURL += "&device_name=%12";

    QStringList deviceInfo = getDeviceInfo();

    auto url = QString(templateURL)
                       .arg(forgetUrl)
                       .arg(QLocale().name())
                       .arg(getThemeName())
                       .arg(getActiveColor())
                       .arg(getStandardFont())
                       .arg(qApp->applicationVersion())
                       .arg(getDeviceKernel())
                       .arg(deviceInfo.at(2))
                       .arg(getOsVersion())
                       .arg(getDeviceCode())
                       .arg(deviceInfo.at(0))
                       .arg(deviceInfo.at(1));

    return url.remove(QRegularExpression("#"));
}

QString wechatURL()
{
    static QString wechatUrl;
    if (wechatUrl.isEmpty()) {
        wechatUrl = getUrlTitle() + QStringLiteral("/view/client/bind-third/wechat");
    }

    QString templateURL = "%1";
    templateURL += "?lang=%2";
    templateURL += "&theme=%3";
    templateURL += "&color=%4";
    templateURL += "&font_family=%5";
    templateURL += "&client_version=%6";
    templateURL += "&device_kernel=%7";
    templateURL += "&device_processor=%8";
    templateURL += "&os_version=%9";
    templateURL += "&device_code=%10";
    templateURL += "&user_name=%11";
    templateURL += "&device_name=%12";

    QStringList deviceInfo = getDeviceInfo();

    auto url = QString(templateURL)
                       .arg(wechatUrl)
                       .arg(QLocale().name())
                       .arg(getThemeName())
                       .arg(getActiveColor())
                       .arg(getStandardFont())
                       .arg(qApp->applicationVersion())
                       .arg(getDeviceKernel())
                       .arg(deviceInfo.at(2))
                       .arg(getOsVersion())
                       .arg(getDeviceCode())
                       .arg(deviceInfo.at(0))
                       .arg(deviceInfo.at(1));

    return url.remove(QRegularExpression("#"));
}

QString getThemeName()
{
    auto themeType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();
    return themeType == Dtk::Gui::DGuiApplicationHelper::DarkType ? "dark" : "light";
}

QString getActiveColor()
{
    QDBusInterface appearance_ifc_("org.deepin.dde.Appearance1",
                                   "/org/deepin/dde/Appearance1",
                                   "org.deepin.dde.Appearance1",
                                   QDBusConnection::sessionBus());
    return appearance_ifc_.property("QtActiveColor").toString();
}

QString getStandardFont()
{
    QDBusInterface appearance_ifc_("org.deepin.dde.Appearance1",
                                   "/org/deepin/dde/Appearance1",
                                   "org.deepin.dde.Appearance1",
                                   QDBusConnection::sessionBus());
    return appearance_ifc_.property("StandardFont").toString();
}

QString getDeviceKernel()
{
    QProcess process;
    process.start("uname", { "-r" });
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    int idx = output.indexOf('\n');
    if (-1 != idx) {
        output.remove(idx, 1);
    }
    return output.data();
}

QString getOsVersion()
{
    QString version =
            QString("%1 (%2)").arg(DSysInfo::uosEditionName()).arg(DSysInfo::minorVersion());
    return version;
}

QString getDeviceCode()
{
    QDBusInterface Interface("com.deepin.deepinid",
                             "/com/deepin/deepinid",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::sessionBus());
    QDBusMessage reply = Interface.call("Get", "com.deepin.deepinid", "HardwareID");
    QList<QVariant> outArgs = reply.arguments();
    QString deviceCode = outArgs.at(0).value<QDBusVariant>().variant().toString();
    return deviceCode;
}

QStringList getDeviceInfo()
{
    qDBusRegisterMetaType<HardwareInfo>();
    QDBusInterface licenseInfo("com.deepin.sync.Helper",
                               "/com/deepin/sync/Helper",
                               "com.deepin.sync.Helper",
                               QDBusConnection::systemBus());

    QDBusReply<HardwareInfo> hardwareInfo = licenseInfo.call(QDBus::AutoDetect, "GetHardware");
    QJsonObject jsonObject;
    auto hardwareInfoValue = hardwareInfo.value();
    auto hardwareDMIValue = hardwareInfo.value().dmi;

    qDebug() << hardwareInfo.error();
    // -pc
    QString userName = hardwareInfoValue.hostName;
    QString Vendor = hardwareDMIValue.boardVendor;
    QString cpu = hardwareInfoValue.cpu;

    QStringList deviceInfo;
    deviceInfo.append(userName);
    deviceInfo.append(Vendor);
    deviceInfo.append(cpu);
    return deviceInfo;
}

QString getEditionName()
{
    return IsCommunitySystem ? "deepin" : "UOS";
}

QString getIconName()
{
    return IsCommunitySystem ? "deepin-id" : "uos-id";
}

}; // namespace utils
