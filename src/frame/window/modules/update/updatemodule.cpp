// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "updatemodule.h"
#include "modules/update/updatemodel.h"
#include "modules/update/updatework.h"
#include "updatewidget.h"
#include "mirrorswidget.h"
#include "modules/systeminfo/systeminfomodel.h"
#include "modules/systeminfo/systeminfowork.h"
#include "widgets/utils.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <QVBoxLayout>
#include <QGSettings>

DCORE_USE_NAMESPACE

using namespace dcc;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateModule::UpdateModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_updateWidget(nullptr)
    , m_mirrorsWidget(nullptr)
{

}

UpdateModule::~UpdateModule()
{
    if (m_workThread) {
        m_workThread->quit();
        m_workThread->wait();
    }
}

void UpdateModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    if (!DSysInfo::isDeepin() || DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        qInfo() << "module: " << displayName() << " is disable now!";
        setAvailable(false);
        return;
    }

    Q_UNUSED(sync);
    Q_UNUSED(pushtype);

    m_workThread = QSharedPointer<QThread>(new QThread);
    m_model = new UpdateModel(this);
    m_work  = QSharedPointer<UpdateWorker>(new UpdateWorker(m_model));
    m_work->moveToThread(m_workThread.get());
    m_workThread->start(QThread::LowPriority);

    connect(m_work.get(), &UpdateWorker::requestInit, m_work.get(), &UpdateWorker::init);
    connect(m_work.get(), &UpdateWorker::requestActive, m_work.get(), &UpdateWorker::activate);
    connect(m_work.get(), &UpdateWorker::requestRefreshLicenseState, m_work.get(), &UpdateWorker::licenseStateChangeSlot);

#ifndef DISABLE_SYS_UPDATE_MIRRORS
    connect(m_work.get(), &UpdateWorker::requestRefreshMirrors, m_work.get(), &UpdateWorker::refreshMirrors);
#endif
    // 之前自动更新与更新提醒后端为同一处理逻辑，新需求分开处理，前端相应提示角标处理逻辑同步调整
    connect(m_model, &UpdateModel::updateNotifyChanged, this, [this](const bool state) {
        //关闭“自动提醒”，隐藏提示角标
        if (!state) {
            m_frameProxy->setModuleSubscriptVisible(name(), false);
        } else {
            UpdatesStatus status = m_model->status();
            if (status == UpdatesStatus::UpdatesAvailable || status == UpdatesStatus::Updateing || status == UpdatesStatus::Downloading || status == UpdatesStatus::DownloadPaused || status == UpdatesStatus::Downloaded ||
                    status == UpdatesStatus::Installing || status == UpdatesStatus::RecoveryBackingup || status == UpdatesStatus::RecoveryBackingSuccessed || m_model->getUpdatablePackages()) {
                m_frameProxy->setModuleSubscriptVisible(name(), true);
            }
        }
    });
    connect(m_model, &UpdateModel::statusChanged, this, &UpdateModule::notifyDisplayReminder);

    // 初始化更新小红点处理
    onUpdatablePackagesChanged(m_model->getUpdatablePackages());
    connect(m_model, &UpdateModel::updatablePackagesChanged, this, &UpdateModule::onUpdatablePackagesChanged);

    if (DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        m_frameProxy->setModuleVisible(this, false);
        setDeviceUnavailabel(true);
    } else {
        bool bShowUpdate = valueByQSettings<bool>(DCC_CONFIG_FILES, "", "showUpdate", true);
        m_frameProxy->setModuleVisible(this, bShowUpdate);
        setDeviceUnavailabel(!bShowUpdate);
    }

#ifndef DISABLE_ACTIVATOR
    connect(m_model, &UpdateModel::systemActivationChanged, this, [ = ](UiActiveState systemactivation) {
        if (systemactivation == UiActiveState::Authorized || systemactivation == UiActiveState::TrialAuthorized || systemactivation == UiActiveState::AuthorizedLapse) {
            if (m_updateWidget)
                m_updateWidget->setSystemVersion(m_model->systemVersionInfo());
        }
    });
#endif

    Q_EMIT m_work->requestInit();
    Q_EMIT m_work->requestActive();

    addChildPageTrans();
    initSearchData();
}

void UpdateModule::initialize()
{
}

const QString UpdateModule::name() const
{
    return QStringLiteral("update");
}

const QString UpdateModule::displayName() const
{
    return tr("Updates");
}

void UpdateModule::active()
{
    qDebug() << "UpdateModule::active";
    connect(m_model, &UpdateModel::beginCheckUpdate, m_work.get(), &UpdateWorker::checkForUpdates);
    connect(m_model, &UpdateModel::updateHistoryAppInfos, m_work.get(), &UpdateWorker::refreshHistoryAppsInfo, Qt::DirectConnection);
    connect(m_model, &UpdateModel::updateCheckUpdateTime, m_work.get(), &UpdateWorker::refreshLastTimeAndCheckCircle, Qt::DirectConnection);

    m_updateWidget = new UpdateWidget;
    m_updateWidget->setVisible(false);
    m_updateWidget->initialize();

    Q_EMIT m_work->requestRefreshLicenseState();

    if (m_model->systemActivation() == UiActiveState::Authorized || m_model->systemActivation() == UiActiveState::TrialAuthorized || m_model->systemActivation() == UiActiveState::AuthorizedLapse) {
        m_updateWidget->setSystemVersion(m_model->systemVersionInfo());
    }
    qDebug() << "UpdateModule::active setModel";
    m_updateWidget->setModel(m_model, m_work.get());

    connect(m_updateWidget, &UpdateWidget::pushMirrorsView, this, [ = ]() {
        m_mirrorsWidget = new MirrorsWidget(m_model);
        m_mirrorsWidget->setVisible(false);
        int topWidgetWidth = m_updateWidget->parentWidget()->parentWidget()->width();
        m_work->checkNetselect();
        m_mirrorsWidget->setMinimumWidth(topWidgetWidth / 2);
        m_mirrorsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        connect(m_mirrorsWidget, &MirrorsWidget::requestSetDefaultMirror, m_work.get(), &UpdateWorker::setMirrorSource);
        connect(m_mirrorsWidget, &MirrorsWidget::requestTestMirrorSpeed, m_work.get(), &UpdateWorker::testMirrorSpeed);
        connect(m_mirrorsWidget, &MirrorsWidget::notifyDestroy, this, [this]() {
            //notifyDestroy信号是此对象被销毁，析构时发出的，资源销毁了要将其对象赋值为空
            m_mirrorsWidget = nullptr;
        });
        connect(m_model, &UpdateModel::smartMirrorSwitchChanged, this, &UpdateModule::onNotifyDealMirrorWidget);

        //mainWidget->parentWidget()->parentWidget()即mainwindow
        //1690是能正常完全显示二级页面的宽度(包括注释说明文字)
        if (topWidgetWidth <= 1690) {
            m_frameProxy->pushWidget(this, m_mirrorsWidget, dccV20::FrameProxyInterface::PushType::DirectTop);
        } else {
            m_frameProxy->pushWidget(this, m_mirrorsWidget);
        }
        m_mirrorsWidget->setVisible(true);
    });

    connect(m_updateWidget, &UpdateWidget::requestLastoreHeartBeat, m_work.get(), &UpdateWorker::onRequestLastoreHeartBeat);

#ifndef DISABLE_ACTIVATOR
    if (m_model->systemActivation() == UiActiveState::Authorized || m_model->systemActivation() == UiActiveState::TrialAuthorized || m_model->systemActivation() == UiActiveState::AuthorizedLapse) {
        m_updateWidget->setSystemVersion(m_model->systemVersionInfo());
    }
#else
    mainWidget->setSystemVersion(m_model->systemVersionInfo());
#endif

    m_frameProxy->pushWidget(this, m_updateWidget);
    m_updateWidget->setVisible(true);
    m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
}

void UpdateModule::deactive()
{
    if (m_model) {
        m_model->deleteLater();
        m_model = nullptr;
    }

    if (m_work) {
        m_work->deleteLater();
        m_work = nullptr;
    }
}

int UpdateModule::load(const QString &path)
{
    int hasPage = -1;
    if (m_updateWidget) {
        if (path == "Update Settings") {
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSetting);
        } else if (path == "Update") {
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
        } else if (path == "Update Settings/Mirror List") {
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateSettingMir);
        } else if (path == "Checking") {
            m_model->setStatus(UpdatesStatus::Checking);
            hasPage = 0;
            m_updateWidget->refreshWidget(UpdateWidget::UpdateType::UpdateCheck);
        }
    }

    return hasPage;
}

QStringList UpdateModule::availPage() const
{
    return QStringList() << "Update Settings" << "Update" << "Update Settings/Mirror List" << "Checking";
}

void UpdateModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //update
        m_frameProxy->addChildPageTrans("Check for Updates", tr("Check for Updates"));
        m_frameProxy->addChildPageTrans("Update Settings", tr("Update Settings"));
    }
}

void UpdateModule::onNotifyDealMirrorWidget(bool state)
{
    //m_mirrorsWidget存在表示有第三级页面
    if (state && m_mirrorsWidget) {
        m_frameProxy->popWidget(this);
        //popWidget之后就没有第三级页面了,即m_mirrorsWidget为空指针,需要对其地址赋值为nullptr
        //假如是从开启镜像源列表页面后，切换其他页面此时这里资源已经被释放了，但是指针没有赋值为空；再次进入，关闭此处就会直接返回主页面(接收析构时信号)
        m_mirrorsWidget = nullptr;
        //避免第三级页面不存在后,还会处理该函数
        disconnect(m_model, &UpdateModel::smartMirrorSwitchChanged, this, &UpdateModule::onNotifyDealMirrorWidget);
    }
}

void UpdateModule::initSearchData()
{
    const QString& module = displayName();//更新
    const QString& updates = tr("Check for Updates");//检查更新
    const QString& updateSettings = tr("Update Settings");
    static QMap<QString, bool> gsettingsMap;

    auto func_is_visible = [=](const QString &gsettings) {
        if ("" == gsettings) {
            return false;
        }

        bool ret = false;
        ret = GSettingWatcher::instance()->get(gsettings).toString() != "Hidden";
        gsettingsMap.insert(gsettings, ret);
        return ret;
    };

    auto func_process_all = [ = ]() {
        m_frameProxy->setWidgetVisible(module, updates, true);
        m_frameProxy->setDetailVisible(module, updates, tr("Check for Updates"), true);//检查更新

        m_frameProxy->setWidgetVisible(module, updateSettings, true);
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Auto Check for Updates"), func_is_visible("updateAutoCheck"));
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Auto Download Updates"), func_is_visible("updateAutoDownlaod"));
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Auto Install Updates"), m_model->autoDownloadUpdates());
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Updates Notification"), func_is_visible("updateUpdateNotify"));
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Clear Package Cache"), func_is_visible("updateCleanCache"));
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Updates from Repositories"), true);
        m_frameProxy->setDetailVisible(module, updateSettings, tr("App installed in App Store"), false/*func_is_visible("updateAppUpdate")*/);
        m_frameProxy->setDetailVisible(module, updateSettings, tr("System"), func_is_visible("updateSystemUpdate"));
        m_frameProxy->setDetailVisible(module, updateSettings, tr("Security Updates Only"), func_is_visible("updateSecureUpdate"));
     };

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [=](const QString &gsetting, const QString &state) {
        if ("" == gsetting || !gsettingsMap.contains(gsetting)) {
            return;
        }

        if (gsettingsMap.value(gsetting) == (GSettingWatcher::instance()->get(gsetting).toString() != "Hidden")) {
            return;
        }

        bool isVisible = func_is_visible(gsetting);
        if ("updateAutoCheck" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("Auto Check for Updates"), isVisible);
        } else if ("updateAutoDownlaod" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("Auto Download Updates"), isVisible);
        } else if ("updateUpdateNotify" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("Updates Notification"), isVisible);
        } else if ("updateCleanCache" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("Clear Package Cache"), isVisible);
        } else if ("updateAppUpdate" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("App installed in App Store"), isVisible);
        } else if ("updateSystemUpdate" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("System"), isVisible);
        } else if ("updateSecureUpdate" == gsetting) {
            m_frameProxy->setDetailVisible(module, updateSettings, tr("Security Updates Only"), isVisible);
        } else {
            qWarning() << " not contains the gsettings : " << gsetting << state;
            return;
        }

        qInfo() << " [notifyGSettingsChanged]  gsetting, state :" << gsetting << state;
        m_frameProxy->updateSearchData(module);
    });

    func_process_all();
}

void UpdateModule::notifyDisplayReminder(UpdatesStatus status)
{
    if (!m_model->updateNotify()) {
        m_frameProxy->setModuleSubscriptVisible(name(), false);
        return;
    }

    if (status == UpdatesStatus::Checking) {
        //do nothing
    } else if (status == UpdatesStatus::UpdatesAvailable ||
               status == UpdatesStatus::Downloading ||
               status == UpdatesStatus::Updateing ||
               status == UpdatesStatus::DownloadPaused ||
               status == UpdatesStatus::Downloaded ||
               status == UpdatesStatus::Installing ||
               status == UpdatesStatus::RecoveryBackingup ||
               status == UpdatesStatus::RecoveryBackingSuccessed) {
        m_frameProxy->setModuleSubscriptVisible(name(), true);
    } else {
        m_frameProxy->setModuleSubscriptVisible(name(), false);
    }
}

void UpdateModule::onUpdatablePackagesChanged(const bool isUpdatablePackages)
{
    m_frameProxy->setModuleSubscriptVisible(name(), isUpdatablePackages && m_model->updateNotify());
}
