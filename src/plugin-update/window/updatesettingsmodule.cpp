/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "interface/moduleobject.h"
#include "updatesettingsmodule.h"
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
DCC_USE_NAMESPACE

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
            setAutoCheckEnable();
        });
        connect(m_autoCheckUniontechUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::onAutoUpdateCheckChanged);

        m_autoCheckUniontechUpdate->setTitle(tr("System"));
        m_autoCheckUniontechUpdate->addBackground();
        m_autoCheckUniontechUpdate->setChecked(m_model->autoCheckSystemUpdates());
    }));

    appendChild(new WidgetModule<SwitchWidget>("securityUpdatesOnly", tr("Security Updates Only"), [this](SwitchWidget *systemSwitch) {
        m_autoCheckSecureUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoCheckSecureUpdatesChanged, m_autoCheckSecureUpdate, [=](const bool status) {
            m_autoCheckSecureUpdate->setChecked(status);
            setAutoCheckEnable();
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
    appendChild(new WidgetModule<SwitchWidget>("thirdpartyRepositories", tr("Third-party Repositories"), [this](SwitchWidget *systemSwitch) {
        m_autoCheckThirdpartyUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoCheckThirdpartyUpdatesChanged, m_autoCheckThirdpartyUpdate, [=](const bool status) {
            m_autoCheckThirdpartyUpdate->setChecked(status);
            setAutoCheckEnable();
        });
        connect(m_autoCheckThirdpartyUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::onAutoUpdateCheckChanged);

        m_autoCheckThirdpartyUpdate->setTitle(tr("Third-party Repositories"));
        m_autoCheckThirdpartyUpdate->addBackground();
        m_autoCheckThirdpartyUpdate->setChecked(m_model->getAutoCheckThirdpartyUpdates());
    }));

    // 其他设置
    appendChild(new UpdateTitleModule("otherSettings", tr("Other settings")));
    appendChild(new WidgetModule<SwitchWidget>("updateAutoCheck", tr("Auto Check for Updates"), [this](SwitchWidget *systemSwitch) {
        m_autoCheckUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
        connect(m_autoCheckUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoCheckUpdates);

        m_autoCheckUpdate->setTitle(tr("Auto Check for Updates"));
        m_autoCheckUpdate->addBackground();
        m_autoCheckUpdate->setChecked(m_model->autoCheckUpdates());
    }));

    appendChild(new WidgetModule<SwitchWidget>("updateAutoDownlaod", tr("Auto Download Updates"), [this](SwitchWidget *systemSwitch) {
        m_autoDownloadUpdate = systemSwitch;
        connect(m_model, &UpdateModel::autoDownloadUpdatesChanged, m_autoDownloadUpdate, &SwitchWidget::setChecked);
        connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoDownloadUpdates);
        connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, [=](bool state) {
            uiMethodChanged(state ? SettingsMethod::Init : SettingsMethod::autoDownload);
        });

        m_autoDownloadUpdate->setTitle(tr("Auto Download Updates"));
        m_autoDownloadUpdate->addBackground();
        m_autoDownloadUpdate->setChecked(m_model->autoDownloadUpdates());
    }));
    appendChild(new WidgetModule<DTipLabel>("updateAutoDownlaodTips", tr(""), [this](DTipLabel *systemLabel) {
        m_autoDownloadUpdateTips = systemLabel;
        m_autoDownloadUpdateTips->setWordWrap(true);
        m_autoDownloadUpdateTips->setAlignment(Qt::AlignLeft);
        m_autoDownloadUpdateTips->setContentsMargins(10, 0, 10, 0);
        m_autoDownloadUpdateTips->setText(tr("Switch it on to automatically download the updates in wireless or wired network"));
    }));
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

    appendChild(new WidgetModule<SwitchWidget>("updateUpdateNotify", tr("Updates Notification"), [this](SwitchWidget *systemSwitch) {
        m_updateNotify = systemSwitch;
        connect(m_model, &UpdateModel::updateNotifyChanged, m_updateNotify, &SwitchWidget::setChecked);
        connect(m_updateNotify, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetUpdateNotify);

        m_updateNotify->setTitle(tr("Updates Notification"));
        m_updateNotify->addBackground();
        m_updateNotify->setChecked(m_model->updateNotify());
    }));
    appendChild(new WidgetModule<SwitchWidget>("updateCleanCache", tr("Clear Package Cache"), [this](SwitchWidget *systemSwitch) {
        m_autoCleanCache = systemSwitch;
        connect(m_model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);
        connect(m_autoCleanCache, &SwitchWidget::checkedChanged, this, &UpdateSettingsModule::requestSetAutoCleanCache);

        m_autoCleanCache->setTitle(tr("Clear Package Cache"));
        m_autoCleanCache->addBackground();
        m_autoCleanCache->setChecked(m_model->autoCleanCache());
    }));
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

void UpdateSettingsModule::setAutoCheckEnable()
{
    // TODO: 更新全部关闭 显示关闭
    bool enable = (m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());

    auto setCheckEnable = [ = ](QWidget * widget) {
        // TODO: Dconfing to setting
        widget->setEnabled(enable);
    };

    setCheckEnable(m_autoCheckUpdate);
    setCheckEnable(m_autoDownloadUpdate);
    setCheckEnable(m_autoDownloadUpdateTips);
    setCheckEnable(m_autoInstallUpdate);
    setCheckEnable(m_autoInstallUpdatesTips);
    setCheckEnable(m_updateNotify);
}

void UpdateSettingsModule::setUpdateMode()
{
    quint64 updateMode = 0;

    updateMode = updateMode | m_autoCheckSecureUpdate->checked();
    updateMode = (updateMode << 1) | m_autoCheckThirdpartyUpdate->checked();
    updateMode = (updateMode << 1) | m_autoCheckUniontechUpdate->checked();

    setAutoCheckEnable();
    requestSetUpdateMode(updateMode);
}

void UpdateSettingsModule::onAutoUpdateCheckChanged()
{
    if (m_autoCheckSecureUpdate->checked()) {
        m_autoCheckSecureUpdate->setChecked(false);
    }

    setUpdateMode();
}

void UpdateSettingsModule::onAutoSecureUpdateCheckChanged()
{
    if (m_autoCheckSecureUpdate->checked()) {
        m_autoCheckUniontechUpdate->setChecked(false);
    }

    setUpdateMode();
}
