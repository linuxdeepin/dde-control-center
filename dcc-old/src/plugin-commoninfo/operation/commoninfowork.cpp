//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "commoninfowork.h"
#include "commoninfomodel.h"
#include "commoninfoproxy.h"
#include "widgets/utils.h"

#include <DNotifySender>

#include <signal.h>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QSettings>
#include <QLoggingCategory>
#include <QDir>

#include <mutex>

Q_LOGGING_CATEGORY(DccCommonInfoWork, "dcc-commoninfo-work");

std::mutex SCALE_SETTING_GUARD;

static const QString PlyMouthConf = QStringLiteral("/etc/plymouth/plymouthd.conf");

using namespace DCC_NAMESPACE;

const QString &GRUB_EDIT_AUTH_ACCOUNT("root");

const QStringList &SYSTEM_LOCAL_LIST {
    "zh_CN",
    "zh_HK",
    "zh_TW",
    "ug_CN",    // 维语
    "bo_CN"     // 藏语
};

const QMap<QString, QString> &SYSTEM_LOCAL_MAP {
    {"zh_CN", "zh_CN"},
    {"zh_HK", "zh_HK"},
    {"zh_TW", "zh_TW"},
};

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

static void notifyInfo(const QString &summary)
{
    DUtil::DNotifySender(summary)
            .appIcon("dde-control-center")
            .appName(QObject::tr("dde-control-center"))
            .timeOut(5000)
            .call();
}

static void notifyInfoWithBody(const QString &summary, const QString &body)
{
    DUtil::DNotifySender(summary)
            .appIcon("dde-control-center")
            .appName(QObject::tr("dde-control-center"))
            .appBody(body)
            .timeOut(5000)
            .call();
}

CommonInfoWork::CommonInfoWork(CommonInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_commomModel(model)
    , m_commonInfoProxy(new CommonInfoProxy(this))
    , m_title("")
    , m_content("")
    , m_scaleIsSetting(false)
{
    //监听开发者在线认证失败的错误接口信息
    connect(m_commonInfoProxy, &CommonInfoProxy::DeepinIdError, this, &CommonInfoWork::deepinIdErrorSlot);
    connect(m_commonInfoProxy, &CommonInfoProxy::IsLoginChanged, m_commomModel, &CommonInfoModel::setIsLogin);
    connect(m_commonInfoProxy, &CommonInfoProxy::DeviceUnlockedChanged, m_commomModel, &CommonInfoModel::setDeveloperModeState);
    connect(m_commonInfoProxy, &CommonInfoProxy::DefaultEntryChanged, m_commomModel, &CommonInfoModel::setDefaultEntry);
    connect(m_commonInfoProxy, &CommonInfoProxy::EnableThemeChanged, m_commomModel, &CommonInfoModel::setThemeEnabled);
    connect(m_commonInfoProxy, &CommonInfoProxy::TimeoutChanged, m_commomModel, [this] (const uint timeout) {
        m_commomModel->setBootDelay(timeout > 1);
    });
    connect(m_commonInfoProxy, &CommonInfoProxy::UpdatingChanged, m_commomModel, &CommonInfoModel::setUpdating);
    connect(m_commonInfoProxy, &CommonInfoProxy::BackgroundChanged, m_commomModel, [this] () {
        QPixmap pix = QPixmap(m_commonInfoProxy->Background());
        m_commomModel->setBackground(pix);
    });
    connect(m_commonInfoProxy, &CommonInfoProxy::EnabledUsersChanged, m_commomModel, [this] (const QStringList &users) {
        m_commomModel->setGrubEditAuthEnabled(users.contains(GRUB_EDIT_AUTH_ACCOUNT));
    });
    connect(m_commonInfoProxy, &CommonInfoProxy::AuthorizationStateChanged, m_commomModel, [this] (const int code) {
        m_commomModel->setActivation(code == 1 || code == 3);
    });

    connect(m_commonInfoProxy, &CommonInfoProxy::resetEnableTheme, this, [=](){
        m_commomModel->themeEnabledChanged(m_commomModel->themeEnabled());
    });

    connect(m_commonInfoProxy, &CommonInfoProxy::resetGrubEditAuthEnabled, this, [=](){
        m_commomModel->grubEditAuthEnabledChanged(m_commomModel->grubEditAuthEnabled());
    });
}

CommonInfoWork::~CommonInfoWork()
{
    if (m_process) {
        //如果控制中心被强制关闭，需要用kill来杀掉没有被关闭的窗口
        kill(static_cast<__pid_t>(m_process->processId()), 15);
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void CommonInfoWork::active()
{
    m_commomModel->setShowGrubEditAuth(true);
    m_commomModel->setIsLogin(m_commonInfoProxy->IsLogin());
    m_commomModel->setDeveloperModeState(m_commonInfoProxy->DeviceUnlocked());
    m_commomModel->setThemeEnabled(m_commonInfoProxy->EnableTheme());
    m_commomModel->setBootDelay(m_commonInfoProxy->Timeout() > 1);
    m_commomModel->setGrubEditAuthEnabled(m_commonInfoProxy->EnabledUsers().contains(GRUB_EDIT_AUTH_ACCOUNT));
    m_commomModel->setUpdating(m_commonInfoProxy->Updating());
    auto [factor, themeName] = getPlyMouthInformation();
    m_commomModel->setPlymouthScale(factor);
    m_commomModel->setPlymouthTheme(themeName);
    auto AuthorizationState = m_commonInfoProxy->AuthorizationState();
    m_commomModel->setActivation(AuthorizationState == 1 || AuthorizationState == 3);
    m_commomModel->setUeProgram(m_commonInfoProxy->IsEnabled());
    m_commomModel->setEntryLists(m_commonInfoProxy->GetSimpleEntryTitles());
    m_commomModel->setDefaultEntry(m_commonInfoProxy->DefaultEntry());

    QPixmap pix = QPixmap(m_commonInfoProxy->Background());
    m_commomModel->setBackground(pix);
}

void CommonInfoWork::setBootDelay(bool value)
{
    qDebug()<<" CommonInfoWork::setBootDelay  value =  "<< value;
    m_commonInfoProxy->setTimeout(value ? 5 : 1);
}

void CommonInfoWork::setEnableTheme(bool value)
{
    m_commonInfoProxy->setEnableTheme(value);
}

void CommonInfoWork::disableGrubEditAuth()
{
    m_commonInfoProxy->DisableUser(GRUB_EDIT_AUTH_ACCOUNT);
}

void CommonInfoWork::onSetGrubEditPasswd(const QString &password, const bool &isReset)
{
    Q_UNUSED(isReset);
    // 密码加密后发送到后端存储
    m_commonInfoProxy->EnableUser(GRUB_EDIT_AUTH_ACCOUNT, passwdEncrypt(password));
}

void CommonInfoWork::setDefaultEntry(const QString &entry)
{
    m_commonInfoProxy->setDefaultEntry(entry);
}

void CommonInfoWork::setBackground(const QString &path)
{
    m_commonInfoProxy->setBackground(path);
}

void CommonInfoWork::setUeProgram(bool enabled)
{
    QString current_date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm::ss.zzz");
    if (enabled) {
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
        connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=](int result) {
            if (96 == result) {
                m_commonInfoProxy->Enable(enabled);
                m_commomModel->setUeProgram(enabled);
            } else {
                m_commomModel->setUeProgram(!enabled);
                qInfo() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
            }
            m_process->deleteLater();
            m_process = nullptr;
        });
    } else {
        m_commonInfoProxy->Enable(enabled);
        m_commomModel->setUeProgram(enabled);
    }
}

void CommonInfoWork::closeUeProgram()
{
    if (m_process) {
        m_process->kill();
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

    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=](int result) {
        if (96 == result) {
            m_commonInfoProxy->UnlockDevice();
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
    m_commonInfoProxy->Login();
}

QString CommonInfoWork::passwdEncrypt(const QString &password)
{
    const QString &pbkdf2_cmd(R"(echo -e "%1\n%2\n"| grub-mkpasswd-pbkdf2 | grep PBKDF2 | awk '{print $4}')");
    QProcess pbkdf2;
    pbkdf2.start("bash", {"-c", pbkdf2_cmd.arg(password).arg(password)});
    pbkdf2.waitForFinished();
    QString pwdOut = pbkdf2.readAllStandardOutput();
    pwdOut[pwdOut.length() - 1] = '\0';
    return pwdOut;
}

std::pair<int, QString> CommonInfoWork::getPlyMouthInformation()
{
    QSettings settings(PlyMouthConf, QSettings::IniFormat);

    QString themeName = settings.value("Daemon/Theme").toString();

    static QStringList ScaleLowDpiThemeNames = {"deepin-logo", "deepin-ssd-logo", "uos-ssd-logo"};
    static QStringList ScaleHighDpiThemeNames = {"deepin-hidpi-logo", "deepin-hidpi-ssd-logo", "uos-hidpi-ssd-logo"};
    if (ScaleLowDpiThemeNames.contains(themeName)) {
        return {1, themeName};
    } else if (ScaleHighDpiThemeNames.contains(themeName)) {
        return {2, themeName};
    }

    return {0, QString()};
}

void CommonInfoWork::deepinIdErrorSlot(int code, const QString &msg)
{
    Q_UNUSED(code);

    //初始化Notify 七个参数
    QString in0(QObject::tr("dde-control-center"));
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
    m_commonInfoProxy->Notify(in0, in1, in2, in3, in4, in5, in6, in7);
}

void CommonInfoWork::setPlymouthFactor(int factor)
{
    if (factor == m_commomModel->plymouthScale()) {
        return;
    }
    if (m_scaleIsSetting) {
        return;
    }
    std::lock_guard<std::mutex> guard(SCALE_SETTING_GUARD);
    m_scaleIsSetting = true;
    QDBusPendingCall call = m_commonInfoProxy->SetScalePlymouth(factor);
    notifyInfo(tr("Start setting the new boot animation, please wait for a minute"));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, call] {
        if (call.isError()) {
            qCWarning(DccCommonInfoWork) << "DBus Error: " << call.error();
        }
        auto [factor, themeName] = getPlyMouthInformation();
        m_commomModel->setPlymouthTheme(themeName);
        m_commomModel->setPlymouthScale(factor);
        notifyInfoWithBody(tr("Setting new boot animation finished"),
                           tr("The settings will be applied after rebooting the system"));
        m_scaleIsSetting = false;
        watcher->deleteLater();
        Q_EMIT settingScaling(false);
    });
    Q_EMIT settingScaling(true);
}

QPixmap CommonInfoWork::getPlymouthFilePixmap()
{
    QString theme = m_commomModel->plymouthTheme();

    static QString ThemePath = "/usr/share/plymouth/themes";

    QString logoPath = ThemePath + QDir::separator() + theme + QDir::separator() + "logo.png";

    return QPixmap(logoPath);
}
