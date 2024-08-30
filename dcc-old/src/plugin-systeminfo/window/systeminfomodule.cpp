// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "systeminfomodule.h"

#include "hostnameitem.h"
#include "interface/pagemodule.h"
#include "interface/vlistmodule.h"
#include "logoitem.h"
#include "operation/systeminfomodel.h"
#include "operation/systeminfowork.h"
#include "privacypolicywidget.h"
#include "src/frame/utils.h"
#include "userlicensewidget.h"
#include "versionprotocolwidget.h"
#include "widgets/settingsgroupmodule.h"
#include "widgets/titlevalueitem.h"
#include "widgets/widgetmodule.h"
#include "window/utils.h"

#include <dsysinfo.h>
#include <qapplication.h>
#include <qobject.h>
#include <qtimer.h>
#include <qwidget.h>
#include <widgets/utils.h>

#include <QFutureWatcher>
#include <QtConcurrent>

#include <functional>
#include <DIconTheme>
DGUI_USE_NAMESPACE

const QString PLATFORM_NAME = std::visit([]() -> QString {
    QString platformname = QGuiApplication::platformName();
    if (platformname.contains("xcb")) {
        platformname = "X11";
    } else if (platformname.contains("wayland")) {
        platformname = "Wayland";
    }
    return platformname;
});


using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE

SystemInfoModule::SystemInfoModule(QObject *parent)
    : HListModule(parent)
    , m_model(new SystemInfoModel(this))
    , m_work(new SystemInfoWork(m_model, this))
{
    initChildModule();
}

SystemInfoModule::~SystemInfoModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void SystemInfoModule::active()
{
    m_work->activate();
}

// clang-format off
void SystemInfoModule::initChildModule()
{
    // 二级菜单--关于本机
    ModuleObject *moduleAboutPc = new PageModule("aboutThisPc", tr("About This PC"), this);
    appendChild(moduleAboutPc);

    moduleAboutPc->appendChild(new WidgetModule<HostNameItem>("hostName", tr("Computer Name"), this, &SystemInfoModule::initHostnameModule));
    SettingsGroupModule *sysInfoGroup = new SettingsGroupModule("systemInfo", tr("systemInfo"));
    moduleAboutPc->appendChild(sysInfoGroup);

    // systemInfoGroup
    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer
        || DSysInfo::uosType() == DSysInfo::UosType::UosDesktop) {
            sysInfoGroup->appendChild(
                    new WidgetModule<TitleValueItem>("osName", tr("OS Name"), this, &SystemInfoModule::initOSNameModule));
            sysInfoGroup->appendChild(
                    new WidgetModule<TitleValueItem>("version", tr("Version"), this, &SystemInfoModule::initVersionModule));
    }

    sysInfoGroup->appendChild(
            new WidgetModule<TitleValueItem>("edition", tr("Edition"), this, &SystemInfoModule::initEditionModule));
    sysInfoGroup->appendChild(
            new WidgetModule<TitleValueItem>("type", tr("Type"), this, &SystemInfoModule::initTypeModule));
    if (!(IS_COMMUNITY_SYSTEM || DSysInfo::UosEditionUnknown == DSysInfo::uosEditionType())
        && DSysInfo::uosEditionType() != DSysInfo::UosEnterpriseC) {
        sysInfoGroup->appendChild(
                new WidgetModule<TitleAuthorizedItem>("authorization", tr("Authorization"), this, &SystemInfoModule::initAuthorizationModule));
    }
    sysInfoGroup->appendChild(new WidgetModule<TitleValueItem>("processor", tr("Processor"), this, &SystemInfoModule::initProcessorModule));
    sysInfoGroup->appendChild(new WidgetModule<TitleValueItem>("memory", tr("Memory"), this, &SystemInfoModule::initMemoryModule));
    sysInfoGroup->appendChild(new WidgetModule<TitleValueItem>("graphics", tr("Graphics Platform"), this, &SystemInfoModule::initGraphicsPlatformModule));
    sysInfoGroup->appendChild(new WidgetModule<TitleValueItem>("kernel", tr("Kernel"), this, &SystemInfoModule::initKernelModule));
    auto copyrightBottom = new WidgetModule<LogoItem>("", "", this, &SystemInfoModule::initLogoModule);
    copyrightBottom->setExtra(true);

    moduleAboutPc->appendChild(copyrightBottom);

    // 二级菜单--协议与隐私政策
    ModuleObject *moduleAgreement = new VListModule("agreement", tr("Agreements and Privacy Policy"), DIconTheme::findQIcon("dcc_version"), this);

    // 三级菜单--协议与隐私政策-版本协议
    ModuleObject *moduleEdition = new PageModule("editionLicense", tr("Edition License"), DIconTheme::findQIcon("dcc_version"), moduleAgreement);
    moduleEdition->appendChild(new WidgetModule<VersionProtocolWidget>("editionLicense", tr("Edition License"), this, &SystemInfoModule::initGnuLicenseModule));
    moduleAgreement->appendChild(moduleEdition);
    if (auto licensepath = DCC_LICENSE::isEndUserAgreementExist(); licensepath.exist) {
        m_model->setEndUserAgreementPath(licensepath.path);
        // 三级菜单--协议与隐私政策-最终用户许可协议
        ModuleObject *moduleUserAgreement = new PageModule("endUserLicenseAgreement", tr("End User License Agreement"), DIconTheme::findQIcon("dcc_protocol"), moduleAgreement);
        moduleUserAgreement->appendChild(new WidgetModule<UserLicenseWidget>("endUserLicenseAgreement", tr("End User License Agreement"), this, &SystemInfoModule::initUserLicenseModule));
        moduleAgreement->appendChild(moduleUserAgreement);
    }
    // 三级菜单--协议与隐私政策-隐私政策
    ModuleObject *modulePolicy = new PageModule("privacyPolicy", tr("Privacy Policy"), DIconTheme::findQIcon("dcc_privacy_policy"), moduleAgreement);
    modulePolicy->appendChild(new WidgetModule<PrivacyPolicyWidget>());
    moduleAgreement->appendChild(modulePolicy);

    appendChild(moduleAgreement);
}

static bool useDeepinCopyright () {
    static bool useDeepinCopyright = DSysInfo::productType() != DSysInfo::ProductType::Uos;
    return useDeepinCopyright;
}

// clang-format on

const QString systemCopyright()
{
    const QSettings settings("/etc/deepin-installer.conf", QSettings::IniFormat);
    const QString &oem_copyright = settings.value("system_info_vendor_name").toString().toLatin1();
    if (oem_copyright.isEmpty()) {
        if (useDeepinCopyright())
            return QApplication::translate("LogoModule", "Copyright© 2011-%1 Deepin Community")
                    .arg(QString(__DATE__).right(4));
        else
            return QApplication::translate(
                           "LogoModule",
                           "Copyright© 2019-%1 UnionTech Software Technology Co., LTD")
                    .arg(QString(__DATE__).right(4));
    } else {
        return oem_copyright;
    }
}

void SystemInfoModule::initLogoModule(LogoItem *item)
{
    item->setDescription(true);              // 显示文字描述
    item->setDescription(systemCopyright()); // LogoItem构造函数: set the discription visible=false
    item->setLogo(DSysInfo::distributionOrgLogo(DSysInfo::Distribution, DSysInfo::Normal));
}

void SystemInfoModule::initHostnameModule(HostNameItem *item)
{
    // 需要在初始化后进行设置，因为要获取width()属性
    QTimer::singleShot(0, item, [this, item] {
        item->setHostName(m_model->hostName());
    });
    connect(m_model, &SystemInfoModel::hostNameChanged, item, &HostNameItem::setHostName);
    connect(m_model, &SystemInfoModel::setHostNameError, item, &HostNameItem::onSetError);
    connect(item, &HostNameItem::hostNameChanged, m_work, &SystemInfoWork::onSetHostname);
}

void SystemInfoModule::initOSNameModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("OS Name") + ':');
    item->setValue(m_model->productName());
    connect(m_model, &SystemInfoModel::productNameChanged, item, &TitleValueItem::setValue);
}

void SystemInfoModule::initVersionModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Version") + ':');
    item->setValue(m_model->versionNumber());
    connect(m_model, &SystemInfoModel::versionNumberChanged, item, &TitleValueItem::setValue);
}

void SystemInfoModule::initEditionModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Edition") + ':');
    item->setValue(m_model->version());
    connect(m_model, &SystemInfoModel::versionChanged, item, &TitleValueItem::setValue);
}

void SystemInfoModule::initTypeModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Type") + ':');
    item->setValue(tr("%1-bit").arg(m_model->type()));
    connect(m_model, &SystemInfoModel::typeChanged, item, &TitleValueItem::setValue);
}

void setLicenseState(TitleAuthorizedItem *const authorized, DCC_NAMESPACE::ActiveState state)
{
    if (state == Authorized) {
        authorized->setValue(QObject::tr("Activated"));
        authorized->setValueForegroundRole(QColor(21, 187, 24));
        authorized->setButtonText(QObject::tr("View"));
    } else if (state == Unauthorized) {
        authorized->setValue(QObject::tr("To be activated"));
        authorized->setValueForegroundRole(QColor(255, 87, 54));
        authorized->setButtonText(QObject::tr("Activate"));
    } else if (state == AuthorizedLapse) {
        authorized->setValue(QObject::tr("Expired"));
        authorized->setValueForegroundRole(QColor(255, 87, 54));
        authorized->setButtonText(QObject::tr("View"));
    } else if (state == TrialAuthorized) {
        authorized->setValue(QObject::tr("In trial period"));
        authorized->setValueForegroundRole(QColor(255, 170, 0));
        authorized->setButtonText(QObject::tr("Activate"));
    } else if (state == TrialExpired) {
        authorized->setValue(QObject::tr("Trial expired"));
        authorized->setValueForegroundRole(QColor(255, 87, 54));
        authorized->setButtonText(QObject::tr("Activate"));
    }
}

void SystemInfoModule::initAuthorizationModule(TitleAuthorizedItem *item)
{
    item->addBackground();
    item->setTitle(tr("Authorization") + ':');
    setLicenseState(item, m_model->licenseState());
    connect(m_model, &SystemInfoModel::licenseStateChanged, item, [item](ActiveState state) {
        setLicenseState(item, state);
    });
    connect(item, &TitleAuthorizedItem::clicked, m_work, &SystemInfoWork::showActivatorDialog);
}

void SystemInfoModule::initKernelModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Kernel") + ':');
    item->setValue(m_model->kernel());
    connect(m_model, &SystemInfoModel::kernelChanged, item, &TitleValueItem::setValue);
}

void SystemInfoModule::initProcessorModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Processor") + ':');
    item->setValue(m_model->processor());
    connect(m_model, &SystemInfoModel::processorChanged, item, &TitleValueItem::setValue);
}

void SystemInfoModule::initMemoryModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Memory") + ':');
    item->setValue(m_model->memory());
    connect(m_model, &SystemInfoModel::memoryChanged, item, &TitleValueItem::setValue);
}

void SystemInfoModule::initGraphicsPlatformModule(TitleValueItem *item)
{
    item->addBackground();
    item->setTitle(tr("Graphics Platform") + ':');
    item->setValue(PLATFORM_NAME);
}

void SystemInfoModule::initGnuLicenseModule(VersionProtocolWidget *item)
{
    if (m_model->gnuLicense().has_value()) {
        item->setLicense(m_model->gnuLicense().value());
    } else {
        QFutureWatcher<QPair<QString, QString>> *w =
                new QFutureWatcher<QPair<QString, QString>>(this);

        connect(w, &QFutureWatcher<QPair<QString, QString>>::finished, this, [=] {
            const auto r = w->result();
            item->setLicense(r);
        });

        w->setFuture(QtConcurrent::run([this] {
            return m_work->getGNULicenseText();
        }));
    }
}

void SystemInfoModule::initUserLicenseModule(UserLicenseWidget *item)
{
    if (m_model->endUserAgreement().has_value()) {
        item->setUserLicense(m_model->endUserAgreement().value());
    } else {
        QFutureWatcher<QString> *w = new QFutureWatcher<QString>(this);

        connect(w, &QFutureWatcher<QString>::finished, this, [=] {
            const auto r = w->result();
            item->setUserLicense(r);
        });

        w->setFuture(QtConcurrent::run([this] {
            return m_work->getEndUserAgreementText();
        }));
    }
}

QString SystemInfoPlugin::name() const
{
    return QStringLiteral("systeminfo");
}

ModuleObject *SystemInfoPlugin::module()
{
    // 一级菜单--系统信息
    SystemInfoModule *moduleInterface = new SystemInfoModule();
    moduleInterface->setName("systeminfo");
    moduleInterface->setDisplayName(tr("System Info"));
    moduleInterface->setIcon(DIconTheme::findQIcon("dcc_nav_systeminfo"));

    return moduleInterface;
}

QString SystemInfoPlugin::location() const
{
    return "21";
}
