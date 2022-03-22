/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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

#include "commoninfowork.h"
#include "commoninfomodel.h"
#include "widgets/utils.h"

#include <signal.h>
#include <QStandardPaths>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QDBusArgument>
#include <QDBusReply>

DCC_USE_NAMESPACE

const QString GRUB_EDIT_AUTH_ACCOUNT("root");

static const QString getLicensePath(const QString &filePath, const QString &type)
{
    const QString& locale { QLocale::system().name() };
    QString lang = SYSTEM_LOCAL_LIST.contains(locale) ? locale : "en_US";

    QString path = QString(filePath).arg(lang).arg(type);
    if (QFile(path).exists())
        return path;
    else
        return QString(filePath).arg("en_US").arg(type);

}

static QString getUserExpContent()
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

static const QString getDevelopModeLicense(const QString &filePath, const QString &type)
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

CommonInfoWork::CommonInfoWork(CommonInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_commomModel(model)
    , m_title("")
    , m_content("")
{
    m_dBusGrubInter = new QDBusInterface("com.deepin.daemon.Grub2",
                             "/com/deepin/daemon/Grub2",
                             "com.deepin.daemon.Grub2",
                             QDBusConnection::systemBus(), this);

    m_dBusGrubThemeInter = new QDBusInterface("com.deepin.daemon.Grub2",
                                       "/com/deepin/daemon/Grub2/Theme",
                                       "com.deepin.daemon.Grub2.Theme",
                                       QDBusConnection::systemBus(), this);

    m_dBusGrubEditAuthInter = new QDBusInterface("com.deepin.daemon.Grub2",
                                              "/com/deepin/daemon/Grub2/EditAuthentication",
                                              "com.deepin.daemon.Grub2.EditAuthentication",
                                              QDBusConnection::systemBus(), this);

    m_dBusdeepinIdInter = new QDBusInterface("com.deepin.deepinid",
                                                "/com/deepin/deepinid",
                                                "com.deepin.deepinid",
                                                QDBusConnection::sessionBus(), this);

    //dbus属性接口
    m_dBusGrubPropertiesInter = new QDBusInterface("com.deepin.daemon.Grub2",
                                                "/com/deepin/daemon/Grub2",
                                                "org.freedesktop.DBus.Properties",
                                                QDBusConnection::systemBus(), this);

    m_dBusGrubThemePropertiesInter = new QDBusInterface("com.deepin.daemon.Grub2",
                                                "/com/deepin/daemon/Grub2/Theme",
                                                "org.freedesktop.DBus.Properties",
                                                QDBusConnection::systemBus(), this);

    m_dBusGrubEditAuthPropertiesInter = new QDBusInterface("com.deepin.daemon.Grub2",
                                                "/com/deepin/daemon/Grub2/EditAuthentication",
                                                "org.freedesktop.DBus.Properties",
                                                QDBusConnection::systemBus(), this);

    m_dBusdeepinIdPropertiesInter = new QDBusInterface("com.deepin.deepinid",
                                                "/com/deepin/deepinid",
                                                "org.freedesktop.DBus.Properties",
                                                QDBusConnection::sessionBus(), this);


    m_commomModel->setShowGrubEditAuth(true);
    licenseStateChangeSlot();

    if (DSysInfo::UosCommunity != DSysInfo::uosEditionType()) {
        m_dBusUeProgramInter = new QDBusInterface("com.deepin.userexperience.Daemon",
                                                  "/com/deepin/userexperience/Daemon",
                                                  "com.deepin.userexperience.Daemon",
                                                  QDBusConnection::sessionBus(), this);
    }

    bool isLogin = m_dBusdeepinIdPropertiesInter->call("Get", "com.deepin.deepinid", "IsLogin").arguments().at(0).value<QDBusVariant>().variant().toBool();
    bool deviceUnlocked = m_dBusdeepinIdPropertiesInter->call("Get", "com.deepin.deepinid", "DeviceUnlocked").arguments().at(0).value<QDBusVariant>().variant().toBool();
    bool enableTheme = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "EnableTheme").arguments().at(0).value<QDBusVariant>().variant().toBool();
    uint32_t timeout = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "Timeout").arguments().at(0).value<QDBusVariant>().variant().toUInt();
    QStringList enabledUsers = m_dBusGrubEditAuthPropertiesInter->call("Get", "com.deepin.daemon.Grub2.EditAuthentication", "EnabledUsers").arguments().at(0).value<QDBusVariant>().variant().toStringList();
    bool updating = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "Updating").arguments().at(0).value<QDBusVariant>().variant().toBool();
    m_commomModel->setIsLogin(isLogin);
    m_commomModel->setDeveloperModeState(deviceUnlocked);
    m_commomModel->setThemeEnabled(enableTheme);
    m_commomModel->setBootDelay(timeout > 1);
    m_commomModel->setGrubEditAuthEnabled(enabledUsers.contains(GRUB_EDIT_AUTH_ACCOUNT));
    m_commomModel->setUpdating(updating);

    //监听开发者在线认证失败的错误接口信息
    QDBusConnection::sessionBus().connect("com.deepin.deepinid","/com/deepin/deepinid", "com.deepin.deepinid", "Error",
                                         this, SLOT(deepinIdErrorSlot(int, const QString)));
    QDBusConnection::sessionBus().connect("com.deepin.deepinid", "/com/deepin/deepinid", "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
                                         this, SLOT(onDeepinIdPathPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect("com.deepin.daemon.Grub2", "/com/deepin/daemon/Grub2", "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
                                         this, SLOT(onGrubPathPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect("com.deepin.daemon.Grub2", "/com/deepin/daemon/Grub2/Theme", "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
                                         this, SLOT(onGrubThemePathPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect("com.deepin.daemon.Grub2", "/com/deepin/daemon/Grub2/EditAuthentication", "org.freedesktop.DBus.Properties", "PropertiesChanged", "sa{sv}as",
                                         this, SLOT(onGrubEditAuthPathPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect("com.deepin.license", "/com/deepin/license/Info", "com.deepin.license.Info", "LicenseStateChange",
                                         this, SLOT(licenseStateChangeSlot()));
}

CommonInfoWork::~CommonInfoWork()
{
    qDebug() << "~CommonInfoWork";
    if (m_process) {
        //如果控制中心被强制关闭，需要用kill来杀掉没有被关闭的窗口
        kill(static_cast<__pid_t>(m_process->pid()), 15);
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void CommonInfoWork::loadGrubSettings()
{
    if (m_dBusGrubInter->isValid()) {
        grubServerFinished();
    } else {
        QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/", "org.freedesktop.DBus", "StartServiceByName");
        msg << "com.deepin.daemon.Grub2" << quint32(0);
        QDBusPendingReply<quint32> async = QDBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", QDBusConnection::systemBus(), this).connection().asyncCall(msg);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher *w){
            QTimer::singleShot(100, this, &CommonInfoWork::grubServerFinished);
            w->deleteLater();
        });
    }
}

bool CommonInfoWork::defaultUeProgram()
{
    QDBusMessage reply = m_dBusUeProgramInter->call("IsEnabled");
    QList<QVariant> outArgs = reply.arguments();
    return outArgs.at(0).toBool();
}

void CommonInfoWork::setBootDelay(bool value)
{
    qDebug()<<" CommonInfoWork::setBootDelay  value =  "<< value;
    QDBusPendingCall call = m_dBusGrubInter->asyncCall("SetTimeout", value ? 5 : 1);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->bootDelayChanged(m_commomModel->bootDelay());
        }
        w->deleteLater();
    });
}

void CommonInfoWork::setEnableTheme(bool value)
{
    QDBusPendingCall call = m_dBusGrubInter->asyncCall("SetEnableTheme", value);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT m_commomModel->themeEnabledChanged(m_commomModel->themeEnabled());
        }
        onBackgroundChanged();
        w->deleteLater();
    });
}

void CommonInfoWork::disableGrubEditAuth()
{
    QDBusPendingCall call = m_dBusGrubEditAuthInter->asyncCall("Disable", GRUB_EDIT_AUTH_ACCOUNT);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            Q_EMIT grubEditAuthCancel(true);
        }
        w->deleteLater();
    });
}

void CommonInfoWork::onSetGrubEditPasswd(const QString &password, const bool &isReset)
{
    // 密码加密后发送到后端存储
    QDBusPendingCall call = m_dBusGrubEditAuthInter->asyncCall("Enable", GRUB_EDIT_AUTH_ACCOUNT, passwdEncrypt(password));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=](QDBusPendingCallWatcher * w) {
        if (w->isError() && !isReset) {
            Q_EMIT grubEditAuthCancel(false);
        }
        w->deleteLater();
    });
}

void CommonInfoWork::setDefaultEntry(const QString &entry)
{
    QDBusPendingCall call = m_dBusGrubInter->asyncCall("SetDefaultEntry", entry);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (!w->isError()) {
            QString  defaultEntry = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "DefaultEntry").arguments().at(0).value<QDBusVariant>().variant().toString();
            Q_EMIT m_commomModel->defaultEntryChanged(defaultEntry);
        }

        w->deleteLater();
    });
}

void CommonInfoWork::grubServerFinished()
{
    uint32_t timeout = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "Timeout").arguments().at(0).value<QDBusVariant>().variant().toUInt();
    bool enableTheme = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "EnableTheme").arguments().at(0).value<QDBusVariant>().variant().toBool();
    QStringList enabledUsers = m_dBusGrubEditAuthPropertiesInter->call("Get", "com.deepin.daemon.Grub2.EditAuthentication", "EnabledUsers").arguments().at(0).value<QDBusVariant>().variant().toStringList();
    bool updating = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "Updating").arguments().at(0).value<QDBusVariant>().variant().toBool();
    m_commomModel->setBootDelay(timeout > 1);
    m_commomModel->setThemeEnabled(enableTheme);
    m_commomModel->setGrubEditAuthEnabled(enabledUsers.contains(GRUB_EDIT_AUTH_ACCOUNT));
    m_commomModel->setUpdating(updating);

    getEntryTitles();
    onBackgroundChanged();
}

void CommonInfoWork::onBackgroundChanged()
{
    QDBusPendingCall call = m_dBusGrubThemeInter->asyncCall("GetBackground");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &CommonInfoWork::getBackgroundFinished);
}

void CommonInfoWork::onEnabledUsersChanged(const QStringList & value)
{
    Q_UNUSED(value);
    QStringList  enabledUsers = m_dBusGrubEditAuthPropertiesInter->call("Get", "com.deepin.daemon.Grub2.EditAuthentication", "EnabledUsers").arguments().at(0).value<QDBusVariant>().variant().toStringList();
    Q_EMIT m_commomModel->grubEditAuthEnabledChanged(enabledUsers.contains(GRUB_EDIT_AUTH_ACCOUNT));
}

void CommonInfoWork::setBackground(const QString &path)
{
    QDBusPendingCall call = m_dBusGrubThemeInter->asyncCall("SetBackgroundSourceFile", path);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (w->isError()) {
            onBackgroundChanged();
        } else {
            setEnableTheme(true);
        }
        w->deleteLater();
    });
}

void CommonInfoWork::setUeProgram(bool enabled)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
    if (enabled && (defaultUeProgram() != enabled)) {
        qInfo("suser opened experience project switch.");
        // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
        QString allowContent(tr("Agree and Join User Experience Program"));

        // license路径
        m_content = getUserExpContent();

        m_process = new QProcess(this);

        auto pathType = "-c";
        if (!SYSTEM_LOCAL_LIST.contains(QLocale::system().name()))
            pathType = "-e";
        m_process->start("dde-license-dialog",
                                      QStringList() << "-t" << m_title << pathType << m_content << "-a" << allowContent);
        qDebug()<<" Deliver content QStringList() = "<<"dde-license-dialog"
                                                     << "-t" << m_title << pathType << m_content << "-a" << allowContent;
        connect(m_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [=](int result) {
            if (96 == result) {
                if (!m_commomModel->ueProgram()) {
                    m_commomModel->setUeProgram(enabled);
                    qInfo() << QString("On %1, users open the switch to join the user experience program!").arg(current_date);
                }
                m_dBusUeProgramInter->call("Enable", enabled);
            } else {
                m_commomModel->setUeProgram(defaultUeProgram());
                qInfo() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
            }
            m_process->deleteLater();
            m_process = nullptr;
        });
    } else {
        if (defaultUeProgram() != enabled) {
            m_dBusUeProgramInter->call("Enable", enabled);
            qDebug() << QString("On %1, users close the switch to join the user experience program!").arg(current_date);
        }
        if (m_commomModel->ueProgram() != enabled) {
            m_commomModel->setUeProgram(enabled);
            qDebug() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
        }
    }
}

void CommonInfoWork::setEnableDeveloperMode(bool enabled)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");

    if (!enabled)
        return;
    // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
    QString title(tr("The Disclaimer of Developer Mode"));
    QString allowContent(tr("Agree and Request Root Access"));

    // license内容
    QString content = getDevelopModeLicense(":/systeminfo/license/deepin-end-user-license-agreement_developer_community_%1.txt", "");
    QString contentPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).append("tmpDeveloperMode.txt");// 临时存储路径
    QFile *contentFile = new QFile(contentPath);
    // 如果文件不存在，则创建文件
    if (!contentFile->exists()) {
        contentFile->open(QIODevice::WriteOnly);
        contentFile->close();
    }
    // 写入文件内容
    if (!contentFile->open(QFile::ReadWrite | QIODevice::Text | QIODevice::Truncate))
        return;
    contentFile->write(content.toLocal8Bit());
    contentFile->close();

    auto pathType = "-c";
    QStringList sl;
    sl << "zh_CN" << "zh_TW";
    if (!sl.contains(QLocale::system().name()))
        pathType = "-e";

    m_process = new QProcess(this);
    m_process->start("dde-license-dialog", QStringList() << "-t" << title << pathType << contentPath << "-a" << allowContent);

    connect(m_process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [=](int result) {
        if (96 == result) {
            m_dBusdeepinIdInter->call("UnlockDevice");
        } else {
            qInfo() << QString("On %1, Remove developer mode Disclaimer!").arg(current_date);
        }
        contentFile->remove();
        contentFile->deleteLater();
        m_process->deleteLater();
        m_process = nullptr;
    });
}

void CommonInfoWork::login()
{
    Q_ASSERT(m_dBusdeepinIdInter);
    QDBusPendingCall call = m_dBusdeepinIdInter->asyncCall("Login");
    call.waitForFinished();
}

void CommonInfoWork::getEntryTitles()
{
    QDBusPendingCall call = m_dBusGrubInter->asyncCall("GetSimpleEntryTitles");
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ](QDBusPendingCallWatcher * w) {
        if (!w->isError()) {
            QDBusReply<QStringList> reply = w->reply();
            QStringList entries = reply.value();
            m_commomModel->setEntryLists(entries);
            QString defaultEntry = m_dBusGrubPropertiesInter->call("Get", "com.deepin.daemon.Grub2", "DefaultEntry").arguments().at(0).value<QDBusVariant>().variant().toString();
            m_commomModel->setDefaultEntry(defaultEntry);
        } else {
            qDebug() << "get grub entry list failed : " << w->error().message();
        }

        w->deleteLater();
    });
}



void CommonInfoWork::getBackgroundFinished(QDBusPendingCallWatcher *w)
{
    if (!w->isError()) {
        QDBusPendingReply<QString> reply = w->reply();
#if 1
        QPixmap pix = QPixmap(reply.value());
        m_commomModel->setBackground(pix);
#else
        const qreal ratio = qApp->devicePixelRatio();
        QPixmap pix = QPixmap(reply.value()).scaled(QSize(ItemWidth * ratio, ItemHeight * ratio),
                                                    Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);

        const QRect r(0, 0, ItemWidth * ratio, ItemHeight * ratio);
        const QSize size(ItemWidth * ratio, ItemHeight * ratio);

        if (pix.width() > ItemWidth * ratio || pix.height() > ItemHeight * ratio)
            pix = pix.copy(QRect(pix.rect().center() - r.center(), size));

        pix.setDevicePixelRatio(ratio);
        m_commomModel->setBackground(pix);
#endif
    } else {
        qDebug() << w->error().message();
    }

    w->deleteLater();
}

QString CommonInfoWork::passwdEncrypt(const QString &password)
{
    static const QString pbkdf2_cmd(R"(echo -e "%1\n%2\n"| grub-mkpasswd-pbkdf2 | grep PBKDF2 | awk '{print $4}')");
    static QProcess pbkdf2;
    pbkdf2.start("bash", {"-c", pbkdf2_cmd.arg(password).arg(password)});
    pbkdf2.waitForFinished();
    QString pwdOut = pbkdf2.readAllStandardOutput();
    pwdOut[pwdOut.length() - 1] = '\0';
    return pwdOut;
}

void CommonInfoWork::licenseStateChangeSlot()
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(this, &CommonInfoWork::getLicenseState);
    watcher->setFuture(future);
}

void CommonInfoWork::deepinIdErrorSlot(int code, const QString &msg)
{
    Q_UNUSED(code);

    //系统通知弹窗qdbus 接口
    QDBusInterface  tInterNotify("com.deepin.dde.Notification",
                                            "/com/deepin/dde/Notification",
                                            "com.deepin.dde.Notification",
                                            QDBusConnection::sessionBus());

    //初始化Notify 七个参数
    QString in0("dde-control-center");
    uint in1 = 101;
    QString in2("preferences-system");
    QString in3("");
    QString in4("");
    QStringList in5;
    QVariantMap in6;
    int in7 = 5000;

    //截取error接口 1001:未导入证书 1002:未登录 1003:无法获取硬件信息 1004:网络异常 1005:证书加载失败 1006:签名验证失败 1007:文件保存失败
    QString msgcode = msg;
    msgcode = msgcode.split(":").at(0);
    if (msgcode == "1001") {
        in3 = tr("Failed to get root access");
    } else if (msgcode == "1002") {
        in3 = tr("Please sign in to your Union ID first");
    } else if (msgcode == "1003") {
        in3 = tr("Cannot read your PC information");
    } else if (msgcode == "1004") {
        in3 = tr("No network connection");
    } else if (msgcode == "1005") {
        in3 = tr("Certificate loading failed, unable to get root access");
    } else if (msgcode == "1006") {
        in3 = tr("Signature verification failed, unable to get root access");
    } else if (msgcode == "1007") {
        in3 = tr("Failed to get root access");
    }
    //系统通知 认证失败 无法进入开发模式
    tInterNotify.call("Notify", in0, in1, in2, in3, in4, in5, in6, in7);
}

void CommonInfoWork::onDeepinIdPathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
     if (3 != arguments.count()) {
         return;
     }
     QString interfaceName = msg.arguments().at(0).toString();
     if (interfaceName == "com.deepin.deepinid") {
         QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
         QStringList keys = changedProps.keys();
         for (int i = 0; i < keys.size(); i++) {
             if (keys.at(i) == "IsLogin") {
                 m_commomModel->setIsLogin(changedProps.value(keys.at(i)).toBool());
             } else if(keys.at(i) == "DeviceUnlocked") {
                 m_commomModel->setDeveloperModeState(changedProps.value(keys.at(i)).toBool());
             }
         }
     }
}

void CommonInfoWork::onGrubPathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
     if (3 != arguments.count()) {
         return;
     }
     QString interfaceName = msg.arguments().at(0).toString();
     if (interfaceName == "com.deepin.daemon.Grub2") {
         QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
         QStringList keys = changedProps.keys();
         for (int i = 0; i < keys.size(); i++) {
             if (keys.at(i) == "DefaultEntry") {
                 m_commomModel->setDefaultEntry(changedProps.value(keys.at(i)).toString());
             } else if(keys.at(i) == "EnableTheme") {
                 m_commomModel->setThemeEnabled(changedProps.value(keys.at(i)).toBool());
             } else if(keys.at(i) == "Timeout") {
                 m_commomModel->setBootDelay(changedProps.value(keys.at(i)).toInt() > 1);
             } else if(keys.at(i) == "Updating") {
                 m_commomModel->setUpdating(changedProps.value(keys.at(i)).toBool());
             }
         }
     }
}

void CommonInfoWork::onGrubEditAuthPathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
     if (3 != arguments.count()) {
         return;
     }
     QString interfaceName = msg.arguments().at(0).toString();
     if (interfaceName == "com.deepin.daemon.Grub2.EditAuthentication") {
         QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
         QStringList keys = changedProps.keys();
         for (int i = 0; i < keys.size(); i++) {
             if (keys.at(i) == "EnabledUsers") {
                 onEnabledUsersChanged(changedProps.value(keys.at(i)).toStringList());
             }
         }
     }
}


void CommonInfoWork::onGrubThemePathPropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
     if (3 != arguments.count()) {
         return;
     }
     QString interfaceName = msg.arguments().at(0).toString();
     if (interfaceName == "com.deepin.daemon.Grub2.Theme") {
         QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
         QStringList keys = changedProps.keys();
         for (int i = 0; i < keys.size(); i++) {
             if (keys.at(i) == "Background") {
                 onBackgroundChanged();
             }
         }
     }
}

void CommonInfoWork::getLicenseState()
{
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qWarning()<< "com.deepin.license error ,"<< licenseInfo.lastError().name();
        return;
    }

    quint32 reply = licenseInfo.property("AuthorizationState").toUInt();
    qDebug() << "authorize result:" << reply;
    m_commomModel->setActivation(reply == 1 || reply == 3);
}
