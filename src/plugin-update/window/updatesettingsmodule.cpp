//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/moduleobject.h"
#include "updatesettingsmodule.h"
#include "widgets/internalbuttonitem.h"
#include "widgets/settingsgroup.h"
#include "updatemodel.h"
#include "updatework.h"
#include "widgets/titlelabel.h"
#include "widgets/switchwidget.h"
#include "widgets/widgetmodule.h"
#include "dsysinfo.h"
#include "updateplugin.h"

#include <QVBoxLayout>
#include <DTipLabel>
#include <DFontSizeManager>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

UpdateSettingsModule::UpdateSettingsModule(UpdateModel *model, UpdateWorker *work, QObject *parent)
    : PageModule(parent)
    , m_model(model)
    , m_work(work)
{
    setName("updateSettings");
    setDisplayName(tr("Update Settings"));

    initConnection();
    initModuleList();
    uiMethodChanged(SettingsMethod::Init);
}

UpdateSettingsModule::~UpdateSettingsModule()
{

}

void UpdateSettingsModule::deactive()
{

}

void UpdateSettingsModule::initModuleList()
{
    // 可从仓库更新
    appendChild(new UpdateTitleModule("updateSettings", tr("Update Settings")));
    appendChild(new WidgetModule<SwitchWidget>("updateSystemUpdate", tr("System"), [this](SwitchWidget *systemSwitch) {
        m_autoCheckUniontechUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoCheckSystemUpdatesChanged, m_autoCheckUniontechUpdate, [=](const bool status) {
            m_autoCheckUniontechUpdate->setChecked(status);
            setAutoCheckEnable(m_model->autoCheckSecureUpdates() || m_model->getAutoCheckThirdpartyUpdates() || m_autoCheckUniontechUpdate->checked());
        });
        connect(m_autoCheckUniontechUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::onAutoUpdateCheckChanged);

        m_autoCheckUniontechUpdate->setTitle(tr("System"));
        m_autoCheckUniontechUpdate->addBackground();
        m_autoCheckUniontechUpdate->setChecked(m_model->autoCheckSystemUpdates());
    }));

    if (IsProfessionalSystem) {
        appendChild(new WidgetModule<SwitchWidget>("securityUpdatesOnly", tr("Security Updates Only"), [this](SwitchWidget *systemSwitch) {
            m_autoCheckSecureUpdate = systemSwitch;
            connect(m_model, &UpdateModel::autoCheckSecureUpdatesChanged, m_autoCheckSecureUpdate, [=](const bool status) {
                m_autoCheckSecureUpdate->setChecked(status);
                setAutoCheckEnable(m_autoCheckSecureUpdate->checked() || m_autoCheckUniontechUpdate->checked());
            });
            connect(m_autoCheckSecureUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::onAutoSecureUpdateCheckChanged);

            m_autoCheckSecureUpdate->setTitle(tr("Security Updates Only"));
            m_autoCheckSecureUpdate->addBackground();
            m_autoCheckSecureUpdate->setChecked(m_model->autoCheckSecureUpdates());
        }));
        appendChild(new WidgetModule<DTipLabel>("securityUpdatesOnlyTips", tr("Switch it on to only update security vulnerabilities and compatibility issues"), [this](DTipLabel *systemLabel) {
            m_autoCheckSecureUpdateTips = systemLabel;
            m_autoCheckSecureUpdateTips->setWordWrap(true);
            m_autoCheckSecureUpdateTips->setAlignment(Qt::AlignLeft);
            m_autoCheckSecureUpdateTips->setContentsMargins(10, 0, 10, 0);
            m_autoCheckSecureUpdateTips->setText(tr("Switch it on to only update security vulnerabilities and compatibility issues"));
        }));
    }

    if (IsCommunitySystem) {
        appendChild(new WidgetModule<SwitchWidget>("thirdpartyRepositories", tr("Third-party Repositories"), [this](SwitchWidget *systemSwitch) {
            m_autoCheckThirdpartyUpdate = systemSwitch;
            connect(m_model, &UpdateModel::autoCheckThirdpartyUpdatesChanged, m_autoCheckThirdpartyUpdate, [=](const bool status) {
                m_autoCheckThirdpartyUpdate->setChecked(status);
                setAutoCheckEnable(m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());
            });
            connect(m_autoCheckThirdpartyUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::onAutoUpdateCheckChanged);

            m_autoCheckThirdpartyUpdate->setTitle(tr("Third-party Repositories"));
            m_autoCheckThirdpartyUpdate->addBackground();
            m_autoCheckThirdpartyUpdate->setChecked(m_model->getAutoCheckThirdpartyUpdates());
        }));
    }

    // 其他设置
    appendChild(new UpdateTitleModule("otherSettings", tr("Other settings")));
    m_autoCheckUpdateModule = new WidgetModule<SwitchWidget>("updateAutoCheck", tr("Auto Check for Updates"), [this](SwitchWidget *systemSwitch) {
        m_autoCheckUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
        connect(m_autoCheckUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoCheckUpdates);

        m_autoCheckUpdate->setTitle(tr("Auto Check for Updates"));
        m_autoCheckUpdate->addBackground();
        m_autoCheckUpdate->setChecked(m_model->autoCheckUpdates());
    });
    appendChild(m_autoCheckUpdateModule);

    m_autoDownloadUpdateModule = new WidgetModule<SwitchWidget>("updateAutoDownlaod", tr("Auto Download Updates"), [this](SwitchWidget *systemSwitch) {
        m_autoDownloadUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoDownloadUpdatesChanged, m_autoDownloadUpdate, &SwitchWidget::setChecked);
        connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoDownloadUpdates);
        connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, [=](bool state) {
            uiMethodChanged(state ? SettingsMethod::Init : SettingsMethod::autoDownload);
        });

        m_autoDownloadUpdate->setTitle(tr("Auto Download Updates"));
        m_autoDownloadUpdate->addBackground();
        m_autoDownloadUpdate->setChecked(m_model->autoDownloadUpdates());
    });
    appendChild(m_autoDownloadUpdateModule);

    m_autoDownloadUpdateTipsModule = new WidgetModule<DTipLabel>("updateAutoDownlaodTips", tr(""), [this](DTipLabel *systemLabel) {
        m_autoDownloadUpdateTips = systemLabel;
        m_autoDownloadUpdateTips->setWordWrap(true);
        m_autoDownloadUpdateTips->setAlignment(Qt::AlignLeft);
        m_autoDownloadUpdateTips->setContentsMargins(10, 0, 10, 0);
        m_autoDownloadUpdateTips->setText(tr("Switch it on to automatically download the updates in wireless or wired network"));
    });
    appendChild(m_autoDownloadUpdateTipsModule);

    m_autoInstallUpdateModule = new WidgetModule<SwitchWidget>("autoInstallUpdates", tr("Auto Install Updates"), [this](SwitchWidget *systemSwitch) {
        m_autoInstallUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoInstallUpdatesChanged, m_autoInstallUpdate, &SwitchWidget::setChecked);
        connect(m_autoInstallUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoInstall);
        m_autoInstallUpdate->setTitle(tr("Auto Install Updates"));
        m_autoInstallUpdate->addBackground();
        m_autoInstallUpdate->setChecked(m_model->getAutoInstallUpdates());
    });
    appendChild(m_autoInstallUpdateModule);
    m_autoInstallUpdatesTipsModule = new WidgetModule<DTipLabel>("autoInstallUpdatesTips", tr(""), [this](DTipLabel *systemLabel) {
        m_autoInstallUpdatesTips = systemLabel;
        connect(m_model, &UpdateModel::autoInstallUpdateTypeChanged, this, [=](quint64 type) {
            m_autoInstallUpdatesTips->setText(getAutoInstallUpdateType(type));
        });
        m_autoInstallUpdatesTips->setWordWrap(true);
        m_autoInstallUpdatesTips->setAlignment(Qt::AlignLeft);
        m_autoInstallUpdatesTips->setContentsMargins(10, 0, 10, 0);
        m_autoInstallUpdatesTips->setText(getAutoInstallUpdateType(m_model->getAutoInstallUpdateType()));
    });
    appendChild(m_autoInstallUpdatesTipsModule);

    m_updateNotifyModule = new WidgetModule<SwitchWidget>("updateUpdateNotify", tr("Updates Notification"), [this](SwitchWidget *systemSwitch) {
        m_updateNotify = systemSwitch;
        connect(m_model, &UpdateModel::updateNotifyChanged, m_updateNotify, &SwitchWidget::setChecked);
        connect(m_updateNotify, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetUpdateNotify);

        m_updateNotify->setTitle(tr("Updates Notification"));
        m_updateNotify->addBackground();
        m_updateNotify->setChecked(m_model->updateNotify());

    });
    appendChild(m_updateNotifyModule);

    appendChild(new WidgetModule<SwitchWidget>("updateCleanCache", tr("Clear Package Cache"), [this](SwitchWidget *systemSwitch) {
        m_autoCleanCache = systemSwitch;
        connect(m_model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);
        connect(m_autoCleanCache, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoCleanCache);

        m_autoCleanCache->setTitle(tr("Clear Package Cache"));
        m_autoCleanCache->addBackground();
        m_autoCleanCache->setChecked(m_model->autoCleanCache());
    }));

    appendChild(new UpdateTitleModule("InternalUpdateSetting", tr("Updates from Internal Testing Sources")));
    appendChild(new WidgetModule<InternalButtonItem>("internal update", tr("internal update"), [this](InternalButtonItem *internalBtn) {
        internalBtn->addBackground();
        internalBtn->onModelTestingStatusChanged(m_model->getTestingChannelStatus());
        connect(internalBtn, &InternalButtonItem::requestInternalChannel, this, [this, internalBtn](bool enable){
            if (enable) {
                auto url = m_work->getTestingChannelUrl();
                if (url.has_value()) {
                    internalBtn->setLink(url.value());
                    m_work->setTestingChannelEnable(true);
                    return;
                }
                // TODO: maybe warning?
            }
            m_work->setTestingChannelEnable(false);
        });
        connect(m_model, &UpdateModel::TestingChannelStatusChanged, internalBtn, &InternalButtonItem::onModelTestingStatusChanged);
    }));
    auto internalUpdateTip = new WidgetModule<DTipLabel>("internalUpdateTip", tr(""), [](DTipLabel *internalUpdateLabel) {
        internalUpdateLabel->setWordWrap(true);
        internalUpdateLabel->setAlignment(Qt::AlignLeft);
        internalUpdateLabel->setContentsMargins(10, 0, 10, 0);
        internalUpdateLabel->setText(tr("Join Internal Testing Channel"));
    });
    appendChild(internalUpdateTip);
    setAutoCheckEnable(m_model->autoCheckSecureUpdates() || m_model->getAutoCheckThirdpartyUpdates() || m_model->autoCheckSystemUpdates());
}

void UpdateSettingsModule::uiMethodChanged(SettingsMethod uiMethod)
{
    m_autoInstallUpdateModule->setHidden(uiMethod == SettingsMethod::autoDownload);
    m_autoInstallUpdatesTipsModule->setHidden(uiMethod == SettingsMethod::autoDownload);
}


void UpdateSettingsModule::initConnection()
{
    connect(this, &UpdateSettingsModule::requestSetUpdateMode, m_work, &UpdateWorker::setUpdateMode);

    connect(this, &UpdateSettingsModule::requestSetAutoCheckUpdates, m_work, &UpdateWorker::setAutoCheckUpdates);
    connect(this, &UpdateSettingsModule::requestSetAutoDownloadUpdates, m_work, &UpdateWorker::setAutoDownloadUpdates);
    connect(this, &UpdateSettingsModule::requestSetAutoInstall, m_work, &UpdateWorker::setAutoInstallUpdates);
    connect(this, &UpdateSettingsModule::requestSetUpdateNotify, m_work, &UpdateWorker::setUpdateNotify);

    connect(this, &UpdateSettingsModule::requestSetAutoCleanCache, m_work, &UpdateWorker::setAutoCleanCache);
}

QString UpdateSettingsModule::getAutoInstallUpdateType(quint64 type)
{
    QString text = "";
    if (type & ClassifyUpdateType::SystemUpdate) {
        text = tr("System Updates");
    }
    if (type & ClassifyUpdateType::SecurityUpdate) {
        if (text.isEmpty()) {
            text += tr("Security Updates");
        } else {
            text = text + "," + tr("Security Updates");
        }
    }
    if (type & ClassifyUpdateType::UnknownUpdate) {
        if (text.isEmpty()) {
            text += tr("Third-party Repositories");
        } else {
            text = text + "," + tr("Third-party Repositories");
        }
    }

    if (DSysInfo::isCommunityEdition()) {
        text = tr("Install updates automatically when the download is complete");
    } else {
        text = QString(tr("Install \"%1\" automatically when the download is complete").arg(text));
    }
    return text;
}

void UpdateSettingsModule::setAutoCheckEnable(bool checkstatus)
{
    auto setCheckEnable = [ = ](DCC_NAMESPACE::ModuleObject  * widgetModule) {
        widgetModule->setEnabled(checkstatus);
    };

    setCheckEnable(m_autoCheckUpdateModule);
    setCheckEnable(m_autoDownloadUpdateModule);
    setCheckEnable(m_autoDownloadUpdateTipsModule);
    setCheckEnable(m_autoInstallUpdateModule);
    setCheckEnable(m_autoInstallUpdatesTipsModule);
    setCheckEnable(m_updateNotifyModule);
}

void UpdateSettingsModule::setUpdateMode()
{
    quint64 updateMode = 0;
    bool autoCheck = IsProfessionalSystem ? m_autoCheckSecureUpdate->checked() : false;
    updateMode = updateMode | autoCheck;
    bool checkThird = IsCommunitySystem ? m_autoCheckThirdpartyUpdate->checked() : false;
    updateMode = (updateMode << 1) | checkThird;
    updateMode = (updateMode << 2);
    updateMode = (updateMode << 1) | m_autoCheckUniontechUpdate->checked();

    setAutoCheckEnable(m_model->autoCheckSecureUpdates() || m_model->getAutoCheckThirdpartyUpdates() || m_model->autoCheckSystemUpdates());
    requestSetUpdateMode(updateMode);
}

void UpdateSettingsModule::onAutoUpdateCheckChanged()
{
    if (IsProfessionalSystem && m_autoCheckSecureUpdate->checked()) {
        m_autoCheckSecureUpdate->setChecked(false);
    }

    setUpdateMode();
}

void UpdateSettingsModule::onAutoSecureUpdateCheckChanged()
{
    if (IsProfessionalSystem && m_autoCheckSecureUpdate->checked()) {
        m_autoCheckUniontechUpdate->setChecked(false);
    }

    setUpdateMode();
}
