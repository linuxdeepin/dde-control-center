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

#include "updatesettings.h"
#include "widgets/settingsgroup.h"
#include "modules/update/updatemodel.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/smalllabel.h"
#include "widgets/titlelabel.h"
#include "widgets/switchwidget.h"
#include "widgets/nextpagewidget.h"
#include "dsysinfo.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DTipLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QGSettings>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateSettings::UpdateSettings(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(nullptr)
    , m_autoCheckUpdate(new SwitchWidget(this))
    , m_autoCheckSecureUpdate(new SwitchWidget(tr("Security Updates Only"), this))
    , m_autoCheckUniontechUpdate(new SwitchWidget(tr("System"), this))
    , m_autoCheckAppUpdate(new SwitchWidget(tr("App installed in App Store"), this))
    , m_updateNotify(new SwitchWidget(tr("Updates Notification"), this))
    , m_autoDownloadUpdate(new SwitchWidget(tr("Download Updates"), this))
    , m_autoInstallUpdate(new SwitchWidget(tr("Auto Install Updates"), this))
    , m_autoInstallUpdatesTips(new DTipLabel(tr("")))
    , m_autoDownloadUpdateTips(new DTipLabel(tr("Switch it on to automatically download the updates in wireless or wired network"), this))
    , m_autoCheckSecureUpdateTips(new DTipLabel(tr("Switch it on to only update security vulnerabilities and compatibility issues"), this))
    , m_autoCleanCache(new SwitchWidget(this))
    , m_dconfig(nullptr)
{
    initUi();
    initConnection();
    setModel(model);
}

UpdateSettings::~UpdateSettings()
{
    GSettingWatcher::instance()->erase("updateAutoCheck", m_autoCheckUpdate);
    GSettingWatcher::instance()->erase("updateUpdateNotify", m_updateNotify);
    GSettingWatcher::instance()->erase("updateAutoDownlaod", m_autoDownloadUpdate);
    GSettingWatcher::instance()->erase("updateCleanCache", m_autoCleanCache);
    GSettingWatcher::instance()->erase("updateSystemUpdate", m_autoCheckUniontechUpdate);
    GSettingWatcher::instance()->erase("updateAppUpdate", m_autoCheckAppUpdate);
    GSettingWatcher::instance()->erase("updateSecureUpdate", m_autoCheckSecureUpdate);
}

void UpdateSettings::initUi()
{
    setTitle(tr("Update Settings"));

    TranslucentFrame *contentWidget = new TranslucentFrame(this); // 添加一层半透明框架
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    //~ contents_path /update/Update Settings
    m_autoCheckUpdate->setTitle(tr("Check for Updates"));

    contentLayout->addSpacing(20);
    SettingsGroup *checkUpdatesGrp = new SettingsGroup;
    checkUpdatesGrp->appendItem(m_autoCheckUpdate);
    checkUpdatesGrp->appendItem(m_autoDownloadUpdate);
    contentLayout->addWidget(checkUpdatesGrp);
    m_autoDownloadUpdateTips->setWordWrap(true);
    m_autoDownloadUpdateTips->setAlignment(Qt::AlignLeft);
    m_autoDownloadUpdateTips->setContentsMargins(10, 0, 10, 0);
    contentLayout->addWidget(m_autoDownloadUpdateTips);
    contentLayout->addSpacing(10);

    SettingsGroup *autoInstallGrp = new SettingsGroup;
    autoInstallGrp->appendItem(m_autoInstallUpdate);
    contentLayout->addWidget(autoInstallGrp);
    m_autoInstallUpdatesTips->setWordWrap(true);
    m_autoInstallUpdatesTips->setAlignment(Qt::AlignLeft);
    m_autoInstallUpdatesTips->setContentsMargins(10, 0, 10, 0);
    contentLayout->addWidget(m_autoInstallUpdatesTips);
    contentLayout->addSpacing(10);

    SettingsGroup *updatesNotificationtGrp = new SettingsGroup;
    updatesNotificationtGrp->appendItem(m_updateNotify);
    m_autoCleanCache->setTitle(tr("Clear Package Cache"));
    updatesNotificationtGrp->appendItem(m_autoCleanCache);
    contentLayout->addWidget(updatesNotificationtGrp);
    contentLayout->addSpacing(20);

    QLabel *autoUpdateSettingsLabel = new QLabel(tr("Updates from Repositories"), this);
    DFontSizeManager::instance()->bind(autoUpdateSettingsLabel, DFontSizeManager::T5, QFont::DemiBold);
    autoUpdateSettingsLabel->setContentsMargins(10, 0, 10, 0); // 左右边距为10
    contentLayout->addWidget(autoUpdateSettingsLabel);
    contentLayout->addSpacing(10);

    SettingsGroup *updatesGrp = new SettingsGroup;
    updatesGrp->appendItem(m_autoCheckUniontechUpdate);
    updatesGrp->appendItem(m_autoCheckAppUpdate);
    updatesGrp->appendItem(m_autoCheckSecureUpdate);
    contentLayout->addWidget(updatesGrp);
    m_autoCheckSecureUpdateTips->setWordWrap(true);
    m_autoCheckSecureUpdateTips->setAlignment(Qt::AlignLeft);
    m_autoCheckSecureUpdateTips->setContentsMargins(10, 0, 10, 0);
    contentLayout->addWidget(m_autoCheckSecureUpdateTips);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (!IsServerSystem && !IsProfessionalSystem && !IsHomeSystem && !IsEducationSystem && !IsDeepinDesktop) {
        //~ contents_path /update/Update Settings
        m_sourceCheck = new SwitchWidget(tr("System Repository Detection"), this);
        m_sourceCheck->addBackground();
        contentLayout->addWidget(m_sourceCheck);
        DTipLabel *sourceCheckTips = new DTipLabel(tr("Show a notification if system update repository has been modified"), this);
        sourceCheckTips->setWordWrap(true);
        sourceCheckTips->setAlignment(Qt::AlignLeft);
        sourceCheckTips->setContentsMargins(10, 0, 10, 0);
        contentLayout->addWidget(sourceCheckTips);
    }
#endif

    if (IsCommunitySystem) {
        //~ contents_path /update/Update Settings
        m_smartMirrorBtn = new SwitchWidget(tr("Smart Mirror Switch"), this);
        m_smartMirrorBtn->addBackground();
        contentLayout->addWidget(m_smartMirrorBtn);

        //~ contents_path /update/Update Settings
        DTipLabel *smartTips = new DTipLabel(tr("Switch it on to connect to the quickest mirror site automatically"), this);
        smartTips->setWordWrap(true);
        smartTips->setAlignment(Qt::AlignLeft);
        smartTips->setContentsMargins(10, 0, 10, 0);
        contentLayout->addWidget(smartTips);

        m_updateMirrors = new NextPageWidget(nullptr, false);
        //~ contents_path /update/Update Settings/Mirror List
        m_updateMirrors->setTitle(tr("Mirror List"));
        m_updateMirrors->setRightTxtWordWrap(true);
        m_updateMirrors->addBackground();
        contentLayout->addWidget(m_updateMirrors);
    }

    contentLayout->setAlignment(Qt::AlignTop);
    contentLayout->setContentsMargins(46, 10, 46, 5);

    setContent(contentWidget);
}

void UpdateSettings::initConnection()
{
    connect(m_autoCheckUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCheckUpdates);
    connect(m_autoCheckSecureUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::onAutoSecureUpdateCheckChanged);
    connect(m_autoCheckUniontechUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::onAutoUpdateCheckChanged);
    connect(m_autoCheckAppUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::onAutoUpdateCheckChanged);
    connect(m_updateNotify, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetUpdateNotify);
    connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoDownloadUpdates);
    connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, [ = ](bool state) {
        setCheckStatus(m_autoInstallUpdate, state, "updateAutoInstall");
    });

    connect(m_autoInstallUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoInstall);
    connect(m_autoCleanCache, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoCleanCache);

#if 0
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (!IsServerSystem && !IsProfessionalSystem && !IsHomeSystem && !IsCommunitySystem && !IsDeepinDesktop) {
        qDebug() << "connect m_sourceCheck";
        connect(m_sourceCheck, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetSourceCheck);
    }
#endif
#endif

    if (IsCommunitySystem) {
        connect(m_updateMirrors, &NextPageWidget::clicked, this, &UpdateSettings::requestShowMirrorsView);
        connect(m_smartMirrorBtn, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestEnableSmartMirror);
    }
}

QString UpdateSettings::getAutoInstallUpdateType(quint64 type)
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
            text += tr("Unknown Apps Updates");
        } else {
            text = text + "," + tr("Unknown Apps Updates");
        }
    }

    text = QString(tr("Install \"%1\" automatically when the download is complete").arg(text));

    return text;
}

void UpdateSettings::setModel(UpdateModel *model)
{
    if (m_model == model)
        return;

    m_model = model;

    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckSecureUpdatesChanged, m_autoCheckSecureUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckSystemUpdatesChanged, m_autoCheckUniontechUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckAppUpdatesChanged, m_autoCheckAppUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::updateNotifyChanged, m_updateNotify, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoDownloadUpdatesChanged, m_autoDownloadUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoDownloadUpdatesChanged, this, [ = ](bool state) {
        setCheckStatus(m_autoInstallUpdate, state, "updateAutoInstall");
        setCheckStatus(m_autoInstallUpdatesTips, state, "updateAutoInstall");
    });
    connect(model, &UpdateModel::autoInstallUpdatesChanged, m_autoInstallUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoInstallUpdateTypeChanged, this, [ = ](quint64 type) {
        m_autoInstallUpdatesTips->setText(getAutoInstallUpdateType(type));
    });
    connect(model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);

    m_autoCheckUpdate->setChecked(model->autoCheckUpdates());
    m_autoCheckSecureUpdate->setChecked(model->autoCheckSecureUpdates());
    m_autoCheckUniontechUpdate->setChecked(model->autoCheckSystemUpdates());
    m_autoCheckAppUpdate->setChecked(model->autoCheckAppUpdates());
    m_updateNotify->setChecked(model->updateNotify());
    m_autoDownloadUpdate->setChecked(model->autoDownloadUpdates());
    setCheckStatus(m_autoInstallUpdate, model->autoDownloadUpdates(), "updateAutoInstall");
    setCheckStatus(m_autoInstallUpdatesTips, model->autoDownloadUpdates(), "updateAutoInstall");
    m_autoCleanCache->setChecked(m_model->autoCleanCache());

    quint64 type = m_model->getAutoInstallUpdateType();
    m_autoInstallUpdatesTips->setText(getAutoInstallUpdateType(type));
    m_autoInstallUpdate->setChecked(model->getAutoInstallUpdates());

    GSettingWatcher::instance()->bind("updateAutoCheck", m_autoCheckUpdate);
    GSettingWatcher::instance()->bind("updateUpdateNotify", m_updateNotify);
    GSettingWatcher::instance()->bind("updateAutoDownlaod", m_autoDownloadUpdate);
    GSettingWatcher::instance()->bind("updateCleanCache", m_autoCleanCache);
    GSettingWatcher::instance()->bind("updateSystemUpdate", m_autoCheckUniontechUpdate);
    m_autoCheckAppUpdate->setVisible(false);

    m_dconfig = DConfigWatcher::instance()->getModulesConfig(DConfigWatcher::update);
    DConfigWatcher::instance()->bind(DConfigWatcher::update, "updateSafety", m_autoCheckSecureUpdate);
    DConfigWatcher::instance()->bind(DConfigWatcher::update, "updateSafety", m_autoCheckSecureUpdateTips);
    connect(m_dconfig, &DConfig::valueChanged, this, [ = ](const QString key) {
        if (key == "updateAutoInstall") {
            setCheckStatus(m_autoInstallUpdate, m_autoDownloadUpdate->checked(), "updateAutoInstall");
            setCheckStatus(m_autoInstallUpdatesTips, m_autoDownloadUpdate->checked(), "updateAutoInstall");
        }
    });

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (!IsServerSystem && !IsProfessionalSystem && !IsHomeSystem && !IsDeepinDesktop) {
        connect(model, &UpdateModel::sourceCheckChanged, m_sourceCheck, &SwitchWidget::setChecked);
        m_sourceCheck->setChecked(model->sourceCheck());
    }
#endif

    if (IsCommunitySystem) {
        auto setDefaultMirror = [this](const MirrorInfo & mirror) {
            m_updateMirrors->setValue(mirror.m_name);
        };

        if (!model->mirrorInfos().isEmpty()) {
            setDefaultMirror(model->defaultMirror());
        }

        connect(model, &UpdateModel::defaultMirrorChanged, this, setDefaultMirror);
        connect(model, &UpdateModel::smartMirrorSwitchChanged, m_smartMirrorBtn, &SwitchWidget::setChecked);
        m_smartMirrorBtn->setChecked(m_model->smartMirrorSwitch());

        auto setMirrorListVisible = [ = ](bool visible) {
            m_updateMirrors->setVisible(!visible);
        };

        connect(model, &UpdateModel::smartMirrorSwitchChanged, this, setMirrorListVisible);
        setMirrorListVisible(model->smartMirrorSwitch());
    }
}

void UpdateSettings::setUpdateMode()
{
    quint64 updateMode = 0;

    updateMode = updateMode | m_autoCheckSecureUpdate->checked();
    updateMode = (updateMode << 3) | m_autoCheckAppUpdate->checked();
    updateMode = (updateMode << 1) | m_autoCheckUniontechUpdate->checked();
    requestSetUpdateMode(updateMode);
}

void UpdateSettings::setCheckStatus(QWidget *widget, bool state, const QString &key)
{
    //后续对应配置相关代码
    const QString status = DConfigWatcher::instance()->getStatus(DConfigWatcher::ModuleType::update, key);

    if ("Enabled" == status) {
        widget->setEnabled(true);
    } else if ("Disabled" == status) {
        widget->setEnabled(false);
    }

    widget->setVisible("Hidden" != status && state);
}

void UpdateSettings::onAutoUpdateCheckChanged()
{
    if (m_autoCheckSecureUpdate->checked()) {
        m_autoCheckSecureUpdate->setChecked(false);
    }

    setUpdateMode();
}

void UpdateSettings::onAutoSecureUpdateCheckChanged()
{
    if (m_autoCheckSecureUpdate->checked()) {
        m_autoCheckAppUpdate->setChecked(false);
        m_autoCheckUniontechUpdate->setChecked(false);
    }

    setUpdateMode();
}
