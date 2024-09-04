//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systeminfowork.h"

#include "systeminfodbusproxy.h"
#include "systeminfomodel.h"
#include "utils.h"

#include <qt5/QtWidgets/QApplication>

#include <DGuiApplicationHelper>
#include <DSysInfo>

#include <QDateTime>
#include <QFutureWatcher>
#include <QtConcurrent/qtconcurrentrun.h>

#include <signal.h>

DCORE_USE_NAMESPACE

namespace DCC_NAMESPACE{

SystemInfoWork::SystemInfoWork(SystemInfoModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_systemInfDBusProxy(new SystemInfoDBusProxy(this))
    , m_process(nullptr)
    , m_content("")
    , m_title("")
{
    qRegisterMetaType<ActiveState>("ActiveState");
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
    } else if (DSysInfo::isDeepin()) {
        version = QString("%1 (%2)").arg(DSysInfo::uosEditionName(), DSysInfo::minorVersion());
    } else {
        version = QString("%1 %2").arg(DSysInfo::productVersion(), DSysInfo::productTypeString());
    }


    m_model->setVersion(version);

    m_model->setType(QSysInfo::WordSize);

    m_model->setKernel(QSysInfo::kernelVersion());
    m_model->setProcessor(DSysInfo::cpuModelName());
    m_model->setMemory(static_cast<qulonglong>(DSysInfo::memoryTotalSize()), static_cast<qulonglong>(DSysInfo::memoryInstalledSize()));

    // 隐私政策文本内容
    QString http = DSysInfo::productType() != DSysInfo::ProductType::Uos ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    QString text = tr("<p>We are deeply aware of the importance of your personal information to you. So we have the Privacy Policy that covers how we collect, use, share, transfer, publicly disclose, and store your information.</p>"
              "<p>You can <a href=\"%1\">click here</a> to view our latest privacy policy and/or view it online by visiting <a href=\"%1\"> %1</a>. Please read carefully and fully understand our practices on customer privacy. If you have any questions, please contact us at: support@uniontech.com.</p>")
               .arg(http);
    m_model->setPrivacyPolicy(text);

    // 用户体验计划内容
    m_model->setJoinUeProgram(m_systemInfDBusProxy->IsEnabled());
    http = IS_COMMUNITY_SYSTEM ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    if (IS_COMMUNITY_SYSTEM) {
        text = tr("<p>Joining User Experience Program means that you grant and authorize us to collect and use the information of your device, system and applications. "
                  "If you refuse our collection and use of the aforementioned information, do not join User Experience Program. "
                  "For details, please refer to Deepin Privacy Policy (<a href=\"%1\"> %1</a>).</p>")
               .arg(http);
    } else {
        text = tr("<p>Joining User Experience Program means that you grant and authorize us to collect and use the information of your device, system and applications. "
                  "If you refuse our collection and use of the aforementioned information, do not join User Experience Program. "
                  "To know more about the management of your data, please refer to UnionTech OS Privacy Policy (<a href=\"%1\"> %1</a>).</p>")
               .arg(http);
    }
    m_model->setUserExperienceProgramText(text);

    m_model->setShowDetail(true);

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

void SystemInfoWork::initSystemCopyright()
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    QString oem_copyright = settings.value("system_info_vendor_name").toString().toLatin1();
    if (oem_copyright.isEmpty()) {
        if (DSysInfo::productType() != DSysInfo::ProductType::Uos)
            //oem_copyright = QString("Copyright© 2011-%1 Deepin Community").arg(QString(__DATE__).right(4));
            oem_copyright = QApplication::translate("LogoModule", "Copyright© 2011-%1 Deepin Community")
                    .arg(QString(__DATE__).right(4));
        else
           // oem_copyright = QString("Copyright© 2019-%1 UnionTech Software Technology Co., LTD").arg(QString(__DATE__).right(4));
            oem_copyright =  QApplication::translate(
                           "LogoModule",
                           "Copyright© 2019-%1 UnionTech Software Technology Co., LTD")
                    .arg(QString(__DATE__).right(4));
    }

    m_model->setSystemCopyright(oem_copyright);
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
                m_systemInfDBusProxy->Enable(enabled);
                m_model->setJoinUeProgram(enabled);
            } else {
                m_model->setJoinUeProgram(!enabled);
                qInfo() << QString("On %1, users cancel the switch to join the user experience program!").arg(current_date);
            }
            m_process->deleteLater();
            m_process = nullptr;
        });
    } else {
        m_systemInfDBusProxy->Enable(enabled);
        m_model->setJoinUeProgram(enabled);
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

}
