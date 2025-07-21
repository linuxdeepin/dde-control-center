//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systeminfowork.h"

#include "systeminfodbusproxy.h"
#include "systeminfomodel.h"
#include "utils.h"

#include <QCoreApplication>

#include <DGuiApplicationHelper>
#include <DSysInfo>
#include <QDBusInterface>

#include <QDateTime>
#include <QFutureWatcher>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QClipboard>

#include <qregularexpression.h>
#include <qtimezone.h>
#include <signal.h>
#include <DDesktopServices>

DCORE_USE_NAMESPACE
DGUI_USE_NAMESPACE

const QString USER_EXPERIENCE_SERVICE = "com.deepin.userexperience.Daemon";


namespace DCC_NAMESPACE{

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_systemInfDBusProxy(new SystemInfoDBusProxy(this))
    , m_process(nullptr)
    , m_content("")
    , m_title("")
    , m_dBusUeProgram(nullptr)
{
    qRegisterMetaType<ActiveState>("ActiveState");
    m_dBusUeProgram = new QDBusInterface(
        "org.deepin.dde.EventLog1",
        "/org/deepin/dde/EventLog1",
        "org.deepin.dde.EventLog1",
        QDBusConnection::sessionBus(), this);

    connect(m_systemInfDBusProxy,
            &SystemInfoDBusProxy::StaticHostnameChanged,
            m_model,
            &SystemInfoModel::setHostName);
    connect(m_systemInfDBusProxy,
            &SystemInfoDBusProxy::AuthorizationStateChanged,
            m_model,
            [this](const int state) {
                m_model->setLicenseState(static_cast<ActiveState>(state));
            });

    connect(m_systemInfDBusProxy,
            &SystemInfoDBusProxy::TimezoneChanged,
            this,
            &SystemInfoWork::onTimezoneChanged);
    connect(m_systemInfDBusProxy,
            &SystemInfoDBusProxy::ShortDateFormatChanged,
            this,
            &SystemInfoWork::onShortDateFormatChanged);


    updateFrequency(false);

}

SystemInfoWork::~SystemInfoWork()
{
    if (m_process) {
        //如果控制中心被强制关闭，需要用kill来杀掉没有被关闭的窗口
        kill(static_cast<__pid_t>(m_process->processId()), 15);
        m_process->deleteLater();
        m_process = nullptr;
    }
}

void SystemInfoWork::activate()
{
    //获取主机名
    m_model->setHostName(m_systemInfDBusProxy->staticHostname());

    m_model->setLogoPath(DSysInfo::distributionOrgLogo(DSysInfo::Distribution, DSysInfo::Normal));
    if (DSysInfo::isDeepin()) {
        m_model->setLicenseState(static_cast<ActiveState>(m_systemInfDBusProxy->authorizationState()));
        QString productName = QString("%1").arg(DSysInfo::uosSystemName());
        m_model->setProductName(productName);
        QString versionNumber = QString("%1").arg(DSysInfo::majorVersion());
        m_model->setVersionNumber(versionNumber);
    }
    QString version;
    if (DSysInfo::uosType() == DSysInfo::UosServer || DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        version = QString("%1%2").arg(DSysInfo::minorVersion(), DSysInfo::uosEditionName());
        m_model->setVersion(version);
    } else if (DSysInfo::isDeepin()) {
        QDBusConnection::systemBus().connect("com.deepin.license",
                                     "/com/deepin/license/Info",
                                     "com.deepin.license.Info",
                                     "LicenseStateChange",
                                     this, SLOT(onLicenseAuthorizationProperty()));

        onLicenseAuthorizationProperty();
    } else {
        version = QString("%1 %2").arg(DSysInfo::productVersion(), DSysInfo::productTypeString());
        m_model->setVersion(version);
    }

    m_model->setType(QSysInfo::WordSize);
    m_model->setKernel(QSysInfo::kernelVersion());
    m_model->setProcessor(DSysInfo::cpuModelName());

    if (m_systemInfDBusProxy->memorySize() > 0) {
        m_model->setMemory(static_cast<qulonglong>(DSysInfo::memoryTotalSize()), m_systemInfDBusProxy->memorySize());
    } else {
        m_model->setMemory(static_cast<qulonglong>(DSysInfo::memoryTotalSize()), static_cast<qulonglong>(DSysInfo::memoryInstalledSize()));
    }

    m_model->setSystemInstallationDate(getSystemInstallDate(m_systemInfDBusProxy->shortDateFormat(), m_systemInfDBusProxy->timezone()));

    // 隐私政策文本内容
    QString http = DSysInfo::productType() != DSysInfo::ProductType::Uos ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    QString text = tr("<p>We are deeply aware of the importance of your personal information to you. So we have the Privacy Policy that covers how we collect, use, share, transfer, publicly disclose, and store your information.</p>"
              "<p>You can <a href=\"%1\">click here</a> to view our latest privacy policy and/or view it online by visiting <a href=\"%1\"> %1</a>. Please read carefully and fully understand our practices on customer privacy. If you have any questions, please contact us at: support@uniontech.com.</p>")
               .arg(http);
    m_model->setPrivacyPolicy(text);

    // 用户体验计划内容
    m_model->setJoinUeProgram(isUeProgramEnabled());

    http = IS_COMMUNITY_SYSTEM ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/experience-en");
    if (IS_COMMUNITY_SYSTEM) {
        text = tr("<p>Joining User Experience Program means that you grant and authorize us to collect and use the information of your device, system and applications. "
                  "If you refuse our collection and use of the aforementioned information, do not join User Experience Program. "
                  "For details, please refer to Deepin Privacy Policy (<a href=\"%1\"> %1</a>).</p>")
               .arg(http);
    } else {
        text = tr("<p>Joining User Experience Program means that you grant and authorize us to collect and use the information of your device, system and applications. "
                  "If you refuse our collection and use of the aforementioned information, please do not join it. For the details of User Experience Program, please visit <a href=\"%1\"> %1</a>.</p>")
               .arg(http);
    }
    m_model->setUserExperienceProgramText(text);

    m_model->setShowDetail(true);

    QString platformname = QGuiApplication::platformName();
    if (platformname.contains("xcb")) {
        platformname = "X11";
    } else if (platformname.contains("wayland")) {
        platformname = "Wayland";
    }
    m_model->setGraphicsPlatform(platformname);

    // 初始化开源协议
    initGnuLicenseData();
    // 初始化用户最终协议
    initUserLicenseData();

    initSystemCopyright();
}

void SystemInfoWork::deactivate() { }

QString SystemInfoWork::getEndUserAgreementText()
{
    if (DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        return DCC_LICENSE::getEulerEndUserAgreement();
    } else {
        if (m_model->endUserAgreementPath().has_value()) {
            return DCC_LICENSE::getEndUserAgreement(m_model->endUserAgreementPath().value());
        }
    }

    return "";
}

void SystemInfoWork::initGnuLicenseData()
{
    QFutureWatcher<QPair<QString, QString>> *w = new QFutureWatcher<QPair<QString, QString>>(this);

    connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [=] {
        const auto r = w->result();
        m_model->setGnuLinceseTitle(r.first);
        m_model->setGnuLinceseContent(r.second);
    });

    w->setFuture(QtConcurrent::run([] {
        return DCC_LICENSE::loadLicenses();
    }));
}

void SystemInfoWork::initUserLicenseData()
{
    DCC_LICENSE::LicenseSearchInfo licensepath = DCC_LICENSE::isEndUserAgreementExist();
    if (licensepath.exist) {
        m_model->setEndUserAgreementPath(licensepath.path);
    }

    QFutureWatcher<QString> *w = new QFutureWatcher<QString>(this);

    connect(w, &QFutureWatcher<QString>::finished, this, [=] {
        const auto r = w->result();
        m_model->setUserLicense(r);
    });

    w->setFuture(QtConcurrent::run([this] {
        return getEndUserAgreementText();
    }));
}

// Removed the placeholder comment
void SystemInfoWork::copyTextToClipboard(const QString &text)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard) {
        clipboard->setText(text);
    }
}

void SystemInfoWork::playSystemSound(int soundType)
{
    DDesktopServices::playSystemSoundEffect(static_cast<DDesktopServices::SystemSoundEffect>(soundType));
}

void SystemInfoWork::initSystemCopyright()
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    QString oem_copyright = settings.value("system_info_vendor_name").toString().toUtf8();
    if (oem_copyright.isEmpty()) {
        if (DSysInfo::productType() != DSysInfo::ProductType::Uos)
            oem_copyright = QCoreApplication::translate("LogoModule", "Copyright© 2011-%1 Deepin Community")
                    .arg(QString(__DATE__).right(4));
        else
            oem_copyright =  QCoreApplication::translate(
                           "LogoModule",
                           "Copyright© 2019-%1 UnionTech Software Technology Co., LTD")
                    .arg(QString(__DATE__).right(4));
    }

    m_model->setSystemCopyright(oem_copyright);
}

void SystemInfoWork::updateFrequency(bool state)
{
    QString validFrequency = "CurrentSpeed";
    QDBusInterface interface("com.deepin.daemon.SystemInfo",
                             "/com/deepin/daemon/SystemInfo",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::sessionBus());
    double cpuMaxMhz = 0.0;
    if (state) {
        validFrequency = "CPUMaxMHz";
    } else {
        QDBusMessage replyCPU = interface.call("Get", "com.deepin.daemon.SystemInfo", "CPUHardware");
        QList<QVariant> outArgsCPU = replyCPU.arguments();
        if (outArgsCPU.count()) {
            QString cpuHardware = outArgsCPU.at(0).value<QDBusVariant>().variant().toString();
            qInfo() << "Current cpu hardware:" << cpuHardware;
            if (cpuHardware.contains("PANGU")) {
                validFrequency = "CPUMaxMHz";
            }
        }
    }
    QDBusMessage reply = interface.call("Get", "com.deepin.daemon.SystemInfo", validFrequency);
    QList<QVariant> outArgs = reply.arguments();
    if (outArgs.count()) {
        cpuMaxMhz = outArgs.at(0).value<QDBusVariant>().variant().toDouble();
    }
    if (DSysInfo::cpuModelName().contains("Hz")) {
        m_model->setProcessor(DSysInfo::cpuModelName());
    } else {
        QString processor;
        QDBusMessage replyCpuInfo = interface.call("Get", "com.deepin.daemon.SystemInfo", "Processor");
        QList<QVariant> outArgsCpuInfo = replyCpuInfo.arguments();
        if (outArgsCpuInfo.count()) {
            processor = outArgsCpuInfo.at(0).value<QDBusVariant>().variant().toString();
        }
        if (processor.contains("Hz")) {
            m_model->setProcessor(processor);
        } else {
            if (DSysInfo::cpuModelName().isEmpty())
                m_model->setProcessor(QString("%1 @ %2GHz").arg(processor)
                                              .arg(cpuMaxMhz / 1000));
            else
                m_model->setProcessor(QString("%1 @ %2GHz").arg(DSysInfo::cpuModelName())
                                              .arg(cpuMaxMhz / 1000));
        }
    }
}

QString SystemInfoWork::getSystemInstallDate(int shortDateFormat, QString timezone)
{
    qDebug() << "ShortDateFormat:" << shortDateFormat << "Timezone:" << timezone;
    QSettings settings("/etc/deepin-installer/deepin-installer.conf", QSettings::NativeFormat);
    const QString &inputTime = settings.value("DI_INSTALL_FINISH_TIME").toString();
    static const QString fotmatShortDate[] = {"yyyy/M/d", "yyyy-M-d", "yyyy.M.d", "yyyy/MM/dd", "yyyy-MM-dd", "yyyy.MM.dd", "MM.dd.yyyy",
                                               "dd.MM.yyyy", "yy/M/d", "yy-M-d", "yy.M.d"};
    int offset = 0;
    QRegularExpression regexUtc("(UTC[+-]\\d{2})");
    QRegularExpressionMatch matchUtc = regexUtc.match(inputTime);
    if (matchUtc.hasMatch()) {
        QRegularExpression regexUtc("([+-]\\d{2})");
        QRegularExpressionMatch matchOffset = regexUtc.match(matchUtc.captured(1));
        if (matchOffset.hasMatch()) {
            offset = matchOffset.captured(1).toInt();
        }
    }
    qDebug() << "Utc offset:" << offset;
    QDateTime dateTime = QDateTime::fromString(inputTime, "yyyy-MM-dd hh:mm:ss 'UTC'tt");

    QTimeZone offsetTimeZone(offset * 3600);
    dateTime.setTimeZone(offsetTimeZone);
    if (dateTime.isValid() && shortDateFormat >= 0 && shortDateFormat <= 10) {
        // 将时间转换到目标时区
        QTimeZone targetTimeZone(timezone.toLocal8Bit());
        qDebug() << "targetTimeZone :" << targetTimeZone.isValid() << timezone;
        QDateTime convertedDateTime = dateTime.toTimeZone(targetTimeZone);
        qDebug() << "Converted DateTime:" << convertedDateTime;
        return convertedDateTime.toString(fotmatShortDate[shortDateFormat]);
    }
    return "";
}

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


void SystemInfoWork::setUeProgram(bool enabled)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
    if (enabled && (isUeProgramEnabled() != enabled)) {
        Q_EMIT requestUeProgram(true);
        qInfo() << "Suser opened experience project switch";
        // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
        QString allowContent(tr("Agree and Join User Experience Program"));

               // license路径
        m_content = getUserExpContent();

        m_process = new QProcess(this);

        auto pathType = "-c";
        const QStringList &sl {
            "zh_CN",
            "zh_HK",
            "zh_TW",
            "ug_CN", // 维语
            "bo_CN" // 藏语
        };
        if (!sl.contains(QLocale::system().name()))
            pathType = "-e";
            
        auto themeType = Dtk::Gui::DGuiApplicationHelper::instance()->themeType();
        QString theme = themeType == Dtk::Gui::DGuiApplicationHelper::DarkType ? "dark" : "light";
        
        m_process->start("dde-license-dialog",
                         QStringList() << "-t" << m_title << pathType << m_content << "-a" << allowContent << "-p" << theme);
        qDebug()<<" Deliver content QStringList() = "<<"dde-license-dialog"
                                                     << "-t" << m_title << pathType << m_content << "-a" << allowContent << "-p" << theme;
        connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, [=](int result) {
            if (96 == result) {
                if (!m_model->joinUeProgram()) {
                    m_model->setJoinUeProgram(enabled);
                    qInfo() << QString("On %1, users open the switch to join the user experience program!").arg(current_date);
                }
                setUeProgramEnabled(enabled);
            } else {
                m_model->setJoinUeProgram(isUeProgramEnabled());
                qInfo() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
            }
            Q_EMIT requestUeProgram(false);
            m_process->deleteLater();
            m_process = nullptr;
        });
    } else {
        if (isUeProgramEnabled() != enabled) {
            setUeProgramEnabled(enabled);
            qDebug() << QString("On %1, users close the switch to join the user experience program!").arg(current_date);
        }
        if (m_model->joinUeProgram() != enabled) {
            m_model->setJoinUeProgram(enabled);
            qDebug() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
        }
    }
}

void SystemInfoWork::showActivatorDialog()
{
    m_systemInfDBusProxy->Show();
}

void SystemInfoWork::showDetail()
{
    m_model->setShowDetail(true);
}

void SystemInfoWork::onSetHostname(const QString &hostname)
{
    m_systemInfDBusProxy->setStaticHostname(hostname, this, SLOT(onSetHostnameFinish()), SLOT(onSetHostnameFinish()));
}

void SystemInfoWork::onSetHostnameFinish()
{
    m_model->setHostName(m_systemInfDBusProxy->staticHostname());
}

void SystemInfoWork::onTimezoneChanged(const QString)
{
    m_model->setSystemInstallationDate(getSystemInstallDate(m_systemInfDBusProxy->shortDateFormat(), m_systemInfDBusProxy->timezone()));
}

void SystemInfoWork::onShortDateFormatChanged(const int)
{
    m_model->setSystemInstallationDate(getSystemInstallDate(m_systemInfDBusProxy->shortDateFormat(), m_systemInfDBusProxy->timezone()));
}

bool SystemInfoWork::isUeProgramEnabled()
{
    if (!m_dBusUeProgram || !m_dBusUeProgram->isValid())
        return false;

    if (m_dBusUeProgram->service() == USER_EXPERIENCE_SERVICE) {
        QDBusMessage reply = m_dBusUeProgram->call("IsEnabled");
        QList<QVariant> outArgs = reply.arguments();
        if (QDBusMessage::ReplyMessage == reply.type() && !outArgs.isEmpty()) {
            return outArgs.first().toBool();
        }
    }

    return m_dBusUeProgram->property("Enabled").toBool();
}

void SystemInfoWork::setUeProgramEnabled(bool enabled)
{

    if (!m_dBusUeProgram || !m_dBusUeProgram->isValid())
        return;

    if (m_dBusUeProgram->service() == USER_EXPERIENCE_SERVICE) {
        m_dBusUeProgram->asyncCall("Enable", enabled);
        return;
    }

    m_dBusUeProgram->asyncCall("Enable", enabled);
}

void SystemInfoWork::onLicenseAuthorizationProperty()
{
    if (DSysInfo::isDeepin()) {
        ActiveState newLicenseState = static_cast<ActiveState>(m_systemInfDBusProxy->authorizationState());
        m_model->setLicenseState(newLicenseState);
    }

    // 从授权服务器获取授权类型
    QString authorizationProperty = getLicenseAuthorizationPropertyString();
    QString version = "";
    if (authorizationProperty != "") {
        version = QString("%1 (%2) (%3)").arg(DSysInfo::uosEditionName())
                .arg(authorizationProperty)
                .arg(DSysInfo::minorVersion());
    } else {
        version = QString("%1 (%2)").arg(DSysInfo::uosEditionName())
                .arg(DSysInfo::minorVersion());
    }

    if (m_model) {
        m_model->setVersion(version);
    }
}

QString SystemInfoWork::getLicenseAuthorizationPropertyString()
{
    QDBusInterface licenseInfo("com.deepin.license",
                               "/com/deepin/license/Info",
                               "com.deepin.license.Info",
                               QDBusConnection::systemBus());

    if (!licenseInfo.isValid()) {
        qWarning() << "Servie: com.deepin.license error: " << licenseInfo.lastError().name();
        return "";
    }

    return licenseInfo.property("AuthorizationPropertyString").toString();
}

}
