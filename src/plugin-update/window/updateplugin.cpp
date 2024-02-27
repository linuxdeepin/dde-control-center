//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "updateplugin.h"
#include "common.h"
#include "updatesettingsmodule.h"
#include "interface/pagemodule.h"

#include "updatemodel.h"
#include "updatewidget.h"
#include "updatectrlwidget.h"
#include "updatework.h"
#include "widgets/titlelabel.h"
#include "widgets/switchwidget.h"

#include <DFontSizeManager>
#include <DIconTheme>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcUpdatePlugin, "dcc-update-plugin")

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

QString UpdatePlugin::name() const
{
    return QStringLiteral("Update");
}

ModuleObject *UpdatePlugin::module()
{
    if (DSysInfo::uosEditionType() == DSysInfo::UosEuler) {
        return nullptr;
    }
    // 一级页面
    UpdateModule *updateInterface = new UpdateModule;

    // 检查更新
    ModuleObject *moduleUpdate = new PageModule("checkForUpdates", tr("Check for Updates"));
    checkUpdateModule *checkUpdatePage = new checkUpdateModule(updateInterface->model(), updateInterface->work(), moduleUpdate);
    moduleUpdate->appendChild(checkUpdatePage);
    updateInterface->appendChild(moduleUpdate);

    // 更新设置
    UpdateSettingsModule *moduleUpdateSettings = new UpdateSettingsModule(updateInterface->model(), updateInterface->work(), updateInterface);
    updateInterface->appendChild(moduleUpdateSettings);

    return updateInterface;
}

QString UpdatePlugin::location() const
{
    return "13";
}

UpdateModule::UpdateModule(QObject *parent)
    : HListModule("update", tr("Updates"), DIconTheme::findQIcon("dcc_nav_update"), parent)
    , m_model(new UpdateModel(this))
    , m_work(new UpdateWorker(m_model, this))
{
    // TODO: 初始化更新小红点处理
    connect(m_model, &UpdateModel::updatablePackagesChanged, this, &UpdateModule::syncUpdatablePackagesChanged);
}

UpdateModule::~UpdateModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void UpdateModule::active()
{
    m_work->activate();
    // 相应授权状态 com.deepin.license.Info
//    m_work->requestRefreshLicenseState();

    connect(m_model, &UpdateModel::beginCheckUpdate, m_work, &UpdateWorker::checkForUpdates);
    connect(m_model, &UpdateModel::updateCheckUpdateTime, m_work, &UpdateWorker::refreshLastTimeAndCheckCircle);
    connect(m_model, &UpdateModel::updateNotifyChanged, this, [this](const bool state) {
        qCDebug(DdcUpdatePlugin) << " ---- updateNotifyChanged" << state;
        //关闭“自动提醒”，隐藏提示角标
        if (!state) {
            syncUpdatablePackagesChanged(false);
        } else {
            UpdatesStatus status = m_model->status();
            if (status == UpdatesStatus::UpdatesAvailable || status == UpdatesStatus::Updating || status == UpdatesStatus::Downloading || status == UpdatesStatus::DownloadPaused || status == UpdatesStatus::Downloaded ||
                    status == UpdatesStatus::Installing || status == UpdatesStatus::RecoveryBackingup || status == UpdatesStatus::RecoveryBackingSuccessed || m_model->getUpdatablePackages()) {
                syncUpdatablePackagesChanged(true);
            }
        }
    });
}

void UpdateModule::deactive()
{
    if (m_model->getTestingChannelStatus() != TestingChannelStatus::Joined) {
        m_model->setTestingChannelStatus(TestingChannelStatus::DeActive);
    }
}

void UpdateModule::syncUpdatablePackagesChanged(const bool isUpdatablePackages)
{
    setBadge(isUpdatablePackages && m_model->updateNotify());
}

QWidget *checkUpdateModule::page()
{
    UpdateWidget *updateWidget = new UpdateWidget;
    updateWidget->setModel(m_model, m_worker);
    connect(updateWidget, &UpdateWidget::requestLastoreHeartBeat, m_worker, &UpdateWorker::onRequestLastoreHeartBeat);
    connect(updateWidget, &UpdateWidget::requestUpdates, m_worker, &UpdateWorker::distUpgrade);
    connect(updateWidget, &UpdateWidget::requestUpdateCtrl, m_worker, &UpdateWorker::OnDownloadJobCtrl);
    connect(updateWidget, &UpdateWidget::requestOpenAppStroe, m_worker, &UpdateWorker::onRequestOpenAppStore);
    connect(updateWidget, &UpdateWidget::requestFixError, m_worker, &UpdateWorker::onFixError);
    updateWidget->displayUpdateContent(UpdateWidget::UpdateType::UpdateCheck);
    return updateWidget;
}

UpdateTitleModule::UpdateTitleModule(const QString &name, const QString &title, QObject *parent)
    : ModuleObject(parent)
{
    setName(name);
    setDescription(title);
    addContentText(title);
}
QWidget *UpdateTitleModule::page()
{
    TitleLabel *titleLabel = new TitleLabel(description());
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
    return titleLabel;
}

SwitchWidgetModule::SwitchWidgetModule(const QString &name, const QString &title, QObject *parent)
    : ModuleObject(parent)
{
    setName(name);
    setDescription(title);
    addContentText(title);
}

QWidget *SwitchWidgetModule::page()
{
    SwitchWidget *sw_widget = new SwitchWidget(description());
    return sw_widget;
}

