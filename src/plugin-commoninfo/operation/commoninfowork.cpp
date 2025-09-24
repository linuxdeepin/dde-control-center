//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "commoninfowork.h"
#include "commoninfomodel.h"
#include "commoninfoproxy.h"
#include "utils.h"

#include <DNotifySender>
#include <DGuiApplicationHelper>

#include <signal.h>
#include <QStandardPaths>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QSettings>
#include <QLoggingCategory>
#include <QDir>
#include <QFileInfo>
#include <DDBusSender>

#include <mutex>
#include <qdbusmetatype.h>
#include <QJsonDocument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>

Q_LOGGING_CATEGORY(DccCommonInfoWork, "dcc-commoninfo-work");

#define IS_COMMUNITY_SYSTEM (DSysInfo::UosCommunity == DSysInfo::uosEditionType())       // 是否是社区版

std::mutex SCALE_SETTING_GUARD;

static const QString PlyMouthConf = QStringLiteral("/etc/plymouth/plymouthd.conf");


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
            .appIcon("")
            .appName("org.deepin.dde.control-center")
            .timeOut(5000)
            .call();
}

static void notifyInfoWithBody(const QString &summary, const QString &body)
{
    DUtil::DNotifySender(summary)
            .appIcon("")
            .appName("org.deepin.dde.control-center")
            .appBody(body)
            .timeOut(5000)
            .call();
}

QDBusArgument &operator<<(QDBusArgument &argument, const DebugArg &debugArg)
{
    argument.beginStructure();
    argument << debugArg.module;
    argument << debugArg.state;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,  DebugArg&debugArg)
{
    argument.beginStructure();
    argument >> debugArg.module;
    argument >> debugArg.state;
    argument.endStructure();
    return argument;
}


CommonInfoWork::CommonInfoWork(CommonInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_commomModel(model)
    , m_commonInfoProxy(new CommonInfoProxy(this))
    , m_title("")
    , m_content("")
    , m_scaleIsSetting(false)
    , m_debugConfigInter(new QDBusInterface("org.deepin.DebugConfig",
                            "/org/deepin/DebugConfig",
                            "org.deepin.DebugConfig",
                            QDBusConnection::systemBus(), this))
    , m_inter(new QDBusInterface("com.deepin.sync.Helper",
                                 "/com/deepin/sync/Helper",
                                 "com.deepin.sync.Helper",
                                 QDBusConnection::systemBus(), this))
{
    //注册类型
    qRegisterMetaType<DebugArg>("DebugArg");
    qDBusRegisterMetaType<DebugArg>();
    qRegisterMetaType<DebugArgList>("DebugArgList");
    qDBusRegisterMetaType<DebugArgList>();

    qRegisterMetaType<DMIInfo>("DMIInfo");
    qDBusRegisterMetaType<DMIInfo>();
    qRegisterMetaType<HardwareInfo>("HardwareInfo");
    qDBusRegisterMetaType<HardwareInfo>();


    //监听开发者在线认证失败的错误接口信息
    connect(m_commonInfoProxy, &CommonInfoProxy::developModeError, this, &CommonInfoWork::onDevelopModeError);
    connect(m_commonInfoProxy, &CommonInfoProxy::IsLoginChanged, m_commomModel, &CommonInfoModel::setIsLogin);
    connect(m_commonInfoProxy, &CommonInfoProxy::DeviceUnlockedChanged, m_commomModel, &CommonInfoModel::setDeveloperModeState);
    connect(m_commonInfoProxy, &CommonInfoProxy::DefaultEntryChanged, m_commomModel, &CommonInfoModel::setDefaultEntry);
    // connect(m_commonInfoProxy, &CommonInfoProxy::EnableThemeChanged, m_commomModel, &CommonInfoModel::setThemeEnabled);
    connect(m_commonInfoProxy, &CommonInfoProxy::EnableThemeChanged, m_commomModel, [this](bool ThemeEnable) {
        m_commomModel->setGrubThemePath(m_commonInfoProxy->Background());
        m_commomModel->setThemeEnabled(ThemeEnable);
    });
    connect(m_commonInfoProxy, &CommonInfoProxy::TimeoutChanged, m_commomModel, [this] (const uint timeout) {
        m_commomModel->setBootDelay(timeout > 1);
    });
    connect(m_commonInfoProxy, &CommonInfoProxy::UpdatingChanged, m_commomModel, &CommonInfoModel::setUpdating);
    connect(m_commonInfoProxy, &CommonInfoProxy::BackgroundChanged, m_commomModel, [this] () {
        if (!m_commomModel->themeEnabled()) {
            setEnableTheme(true);
            m_commomModel->setThemeEnabled(true);
        }
        QString backgroundPath = m_commonInfoProxy->Background();
        QPixmap pix = QPixmap(backgroundPath);
        m_commomModel->setGrubThemePath(backgroundPath);
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

    connect(m_commonInfoProxy, &CommonInfoProxy::resetGrubEditAuthEnabled, this, &CommonInfoWork::resetEditAuthEnabled);

    connect(m_commonInfoProxy, &CommonInfoProxy::DeveloperModeChanged, m_commomModel, &CommonInfoModel::setIsDeveloperMode);
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
    m_commomModel->setIsDeveloperMode(m_commonInfoProxy->DeveloperMode());
    m_commomModel->setThemeEnabled(m_commonInfoProxy->EnableTheme());
    m_commomModel->setBootDelay(m_commonInfoProxy->Timeout() > 1);
    m_commomModel->setGrubEditAuthEnabled(m_commonInfoProxy->EnabledUsers().contains(GRUB_EDIT_AUTH_ACCOUNT));
    m_commomModel->setUpdating(m_commonInfoProxy->Updating());



    auto AuthorizationState = m_commonInfoProxy->AuthorizationState();
    m_commomModel->setActivation(AuthorizationState == 1 || AuthorizationState == 3);
    m_commomModel->setUeProgram(m_commonInfoProxy->IsEnabled());
    m_commomModel->setEntryLists(m_commonInfoProxy->GetSimpleEntryTitles());
    m_commomModel->setDefaultEntry(m_commonInfoProxy->DefaultEntry());

    initGrubAnimationModel();
    initGrubMenuListModel();
    initDebugLogLevel();

    m_commomModel->setGrubThemePath(m_commonInfoProxy->Background());
}

void CommonInfoWork::initGrubAnimationModel()
{
    QList<QPair<int, QString>> scaleThemeList;
    if (IS_COMMUNITY_SYSTEM) {
        scaleThemeList.append(QPair<int, QString>(2, "boot_deepin"));
        scaleThemeList.append(QPair<int, QString>(1, "boot_deepin"));
    } else {
        scaleThemeList.append(QPair<int, QString>(2, "boot_uos"));
        scaleThemeList.append(QPair<int, QString>(1, "boot_uos"));
    }

    auto [factor, themeName] = getPlyMouthInformation();
    m_commomModel->setPlymouthScale(factor);
    m_commomModel->setPlymouthTheme(themeName);

    QList<GrubAnimationData> list;
    for (auto item : scaleThemeList) {
        GrubAnimationData tempData;
        tempData.startAnimation = false;
        tempData.imagePath = item.second;
        tempData.text = item.first == 2 ? tr("Large size") : tr("Small size");
        tempData.plymouthScale = item.first;
        tempData.checkStatus = factor == item.first;
        if (item.first == 2) {
            tempData.scale = 1;
        } else if (item.first == 1) {
            tempData.scale = 0.65;
        } else {
            tempData.scale = 1;
        }
        list.append(tempData);
    }

    m_commomModel->grubAnimationModel()->initData(list);
}

void CommonInfoWork::initGrubMenuListModel()
{
    QList<GrubMenuData> list;
    for (QString item : m_commomModel->entryLists()) {
        GrubMenuData data;
        data.text = item;
        data.checkStatus = QString::compare(m_commomModel->defaultEntry(), item, Qt::CaseSensitive) == 0;
        list.append(data);
    }

    m_commomModel->grubMenuListModel()->initData(list);
}

void CommonInfoWork::initDebugLogLevel()
{
    QStringList arg = {"all"};
    QDBusPendingCall state = m_debugConfigInter->asyncCall("GetState", arg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(state, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher, state] {
        if (!state.isError()) {
            QDBusReply<QStringList> res = state.reply();
            qInfo() << "GetState:" << res.value();

            bool debugState = res.value().first() == "debug";         
            int index = debugState ? 1 : 0;
            m_commomModel->setDebugLogCurrentIndex(index);
        } else {
            qWarning() << "GetState failed:" << state.error();
        }
        watcher->deleteLater();
    });
}

QString CommonInfoWork::verifyPassword(QString text)
{
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword("", text, PwqualityManager::CheckType::Grub2);
    return error != PwqualityManager::ERROR_TYPE::PW_NO_ERR ? PwqualityManager::instance()->getErrorTips(error, PwqualityManager::CheckType::Grub2) : "";
}

void CommonInfoWork::jumpToSecurityCenter()
{
    DDBusSender()
    .service("com.deepin.defender.hmiscreen")
    .interface("com.deepin.defender.hmiscreen")
    .path("/com/deepin/defender/hmiscreen")
    .method(QString("ShowPage"))
    .arg(QString("securitytools"))
    .arg(QString("application-safety"))
    .call();
}

void CommonInfoWork::setLogDebug(int index)
{
    const bool isOn = index == 1;
    const QString &state = isOn ? "debug" : "warning";

    qInfo() << "SetDebug arg:" << state;
    DebugArg arg;
    arg.state = state;
    arg.module = "all";
    DebugArgList argList;
    argList << arg;
    QDBusPendingCall reply = m_debugConfigInter->asyncCall("SetDebug", QVariant::fromValue(argList));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [watcher, reply, this] {
        if (reply.isError()) {
                qWarning() << "SetDebug failed:" << reply.error();
        }
        // Todo 由于org.deepin.DebugConfig后端AllDebugLevel属性变化后没有发送信号，当前临时手动取数据，后端加上信号后改成监听信号
        initDebugLogLevel();
        watcher->deleteLater();
    });
}

void CommonInfoWork::importCertificate(QString filePath)
{
    filePath = filePath.remove("file://");
    //读取机器信息证书
    QFile fFile(filePath);
    if (!fFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open file for writing";
    }

    QByteArray data = fFile.readAll();
    QDBusMessage msg =  m_inter->call("EnableDeveloperMode", data);

           //当返回信息为错误接口信息才处理
    if (msg.type() == QDBusMessage::MessageType::ErrorMessage) {
        //系统通知弹窗qdbus 接口
        QDBusInterface  tInterNotify("org.deepin.dde.Notification1",
                                    "/org/deepin/dde/Notification1",
                                    "org.deepin.dde.Notification1",
                                    QDBusConnection::sessionBus());

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
        QString msgcode = msg.errorMessage();
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

               //系统通知认证失败 无法进入开发模式
        tInterNotify.call("Notify", in0, in1, in2, in3, in4, in5, in6, in7);
    }

}

void CommonInfoWork::exportMessage(QString filePath)
{
    filePath = filePath.remove("file://");
    qDebug() << " importCertificate file path :  " << filePath;
    QDBusInterface licenseInfo("com.deepin.sync.Helper",
                               "/com/deepin/sync/Helper",
                               "com.deepin.sync.Helper",
                               QDBusConnection::systemBus());

    QDBusReply<HardwareInfo> hardwareInfo = licenseInfo.call(QDBus::AutoDetect, "GetHardware");
    QString fileName = filePath;
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite))
        qWarning() << "File open error, file name: " << fileName;
    else
        qInfo() << "Open file: " << fileName;

           // 使用QJsonObject对象插入键值对。
    QJsonObject jsonObject;
    auto hardwareInfoValue = hardwareInfo.value();
    auto hardwareDMIValue = hardwareInfo.value().dmi;
    jsonObject.insert("id", hardwareInfoValue.id);
    jsonObject.insert("hostname", hardwareInfoValue.hostName);
    jsonObject.insert("username", hardwareInfoValue.username);
    jsonObject.insert("cpu", hardwareInfoValue.cpu);
    jsonObject.insert("laptop", hardwareInfoValue.laptop);
    jsonObject.insert("memory", hardwareInfoValue.memory);
    jsonObject.insert("network_cards", hardwareInfoValue.networkCards);

    QJsonObject objectDMI;
    objectDMI.insert("bios_vendor", hardwareDMIValue.biosVendor);
    objectDMI.insert("bios_version", hardwareDMIValue.biosVersion);
    objectDMI.insert("bios_date", hardwareDMIValue.biosDate);
    objectDMI.insert("board_name", hardwareDMIValue.boardName);
    objectDMI.insert("board_serial", hardwareDMIValue.boardSerial);
    objectDMI.insert("board_vendor", hardwareDMIValue.boardVendor);
    objectDMI.insert("board_version", hardwareDMIValue.boardVersion);
    objectDMI.insert("product_name", hardwareDMIValue.productName);
    objectDMI.insert("product_family", hardwareDMIValue.productFamily);
    objectDMI.insert("product_serial", hardwareDMIValue.producctSerial);
    objectDMI.insert("product_uuid", hardwareDMIValue.productUUID);
    objectDMI.insert("product_version", hardwareDMIValue.productVersion);

    jsonObject.insert("dmi", objectDMI);
    //使用QJsonDocument设置该json对象
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);

           //将json以文本形式写入文件并关闭文件
    file.write(jsonDoc.toJson());
    file.close();
}

void CommonInfoWork::resetEditAuthEnabled()
{
    Q_EMIT m_commomModel->grubEditAuthEnabledChanged(m_commomModel->grubEditAuthEnabled());
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
    QFileInfo fileInfo(path);
    QString realPath = path;

    if (fileInfo.exists() && fileInfo.isSymLink()) {
        realPath = fileInfo.symLinkTarget();
        if (QFileInfo(realPath).isRelative()) {
            realPath = fileInfo.dir().absoluteFilePath(realPath);
        }
        qCDebug(DccCommonInfoWork) << "Resolved symlink" << path << "to" << realPath;
    }

    m_commonInfoProxy->setBackground(realPath);
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
        
        auto themeType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();
        QString theme = themeType == Dtk::Gui::DGuiApplicationHelper::DarkType ? "dark" : "light";
        
        m_process->start("dde-license-dialog",
                                      QStringList() << "-t" << m_title << pathType << m_content << "-a" << allowContent << "-p" << theme);
        qDebug()<<" Deliver content QStringList() = "<<"dde-license-dialog"
                                                     << "-t" << m_title << pathType << m_content << "-a" << allowContent << "-p" << theme;
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

    m_commomModel->setNeedShowModalDialog(true);
    auto pathType = "-c";
    QStringList sl;
    sl << "zh_CN" << "zh_TW";
    if (!sl.contains(QLocale::system().name()))
        pathType = "-e";

    auto themeType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();
    QString theme = themeType == Dtk::Gui::DGuiApplicationHelper::DarkType ? "dark" : "light";

    m_process = new QProcess(this);
    m_process->start("dde-license-dialog", QStringList() << "-t" << title << pathType << contentPath << "-a" << allowContent << "-p" << theme);

    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=](int result) {
        if (96 == result) {
            m_commonInfoProxy->UnlockDevice();
        } else {
            qInfo() << QString("On %1, Remove developer mode Disclaimer!").arg(current_date);
        }
        m_commomModel->setNeedShowModalDialog(false);
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

void CommonInfoWork::onDevelopModeError(const QString &msgCode)
{
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
    if (msgCode == "1001") {
        in3 = tr("Failed to get root access");
    } else if (msgCode == "1002") {
        in3 = tr("Please sign in to your Union ID first");
    } else if (msgCode == "1003") {
        in3 = tr("Cannot read your PC information");
    } else if (msgCode == "1004") {
        in3 = tr("No network connection");
    } else if (msgCode == "1005") {
        in3 = tr("Certificate loading failed, unable to get root access");
    } else if (msgCode == "1006") {
        in3 = tr("Signature verification failed, unable to get root access");
    } else if (msgCode == "1007") {
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

    m_commomModel->grubAnimationModel()->updateCheckIndex(factor, true);
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

bool CommonInfoWork::isSecurityCenterInstalled()
{
    auto isDbusNameAvailable = [](const QDBusConnection &conn, const QString &serviceName) -> bool {
        auto *iface = conn.interface();
        if (!iface)
            return false;
        return iface->isServiceRegistered(serviceName);
    };

    if (isDbusNameAvailable(QDBusConnection::sessionBus(), QStringLiteral("com.deepin.defender.hmiscreen")))
        return true;

    if (isDbusNameAvailable(QDBusConnection::systemBus(), QStringLiteral("com.deepin.defender.AutostartManager")))
        return true;

    return QFileInfo::exists("/usr/libexec/deepin/deepin-defender");
}

bool CommonInfoWork::isACLController() const
{
    return m_commonInfoProxy->isACLController();
}
