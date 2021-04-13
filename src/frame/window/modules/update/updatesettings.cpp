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

#include <DTipLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>

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
    , m_autoCheckSecureUpdate(new SwitchWidget(tr("Security Updates"), this))
    , m_autoCheckSystemUpdate(new SwitchWidget(tr("System Updates"), this))
    , m_autoCheckAppUpdate(new SwitchWidget(tr("App Updates in App Store"), this))
    , m_updateNotify(new SwitchWidget(tr("Updates Notification"), this))
    , m_autoDownloadUpdate(new SwitchWidget(tr("Download Updates"), this))
    , m_autoDownloadUpdateTips(new DTipLabel(tr("Switch it on to automatically download the updates in wireless or wired network"), this))
    , m_autoCleanCache(new SwitchWidget(this))
{
    initUi();
    initConnection();
    setModel(model);
}

void UpdateSettings::initUi()
{
    setTitle(tr("Update Settings"));

    TranslucentFrame *contentWidget = new TranslucentFrame(this); // 添加一层半透明框架
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    QLabel *autoUpdateSettingsLabel = new QLabel(tr("Automatic Updating Settings"), this);
    DFontSizeManager::instance()->bind(autoUpdateSettingsLabel, DFontSizeManager::T5, QFont::DemiBold);
    autoUpdateSettingsLabel->setContentsMargins(10, 0, 10, 0); // 左右边距为10
    contentLayout->addWidget(autoUpdateSettingsLabel);
    contentLayout->addSpacing(10);

    //~ contents_path /update/Update Settings
    m_autoCheckUpdate->setTitle(tr("Check for Updates"));

    SettingsGroup *checkUpdatesGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    checkUpdatesGrp->appendItem(m_autoCheckUpdate);
    checkUpdatesGrp->appendItem(m_autoCheckSecureUpdate);
    checkUpdatesGrp->appendItem(m_autoCheckSystemUpdate);
    checkUpdatesGrp->appendItem(m_autoCheckAppUpdate);
    contentLayout->addWidget(checkUpdatesGrp);
    contentLayout->addSpacing(20);

    SettingsGroup *updatesNotificationtGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    updatesNotificationtGrp->appendItem(m_updateNotify);
    updatesNotificationtGrp->appendItem(m_autoDownloadUpdate);
    contentLayout->addWidget(updatesNotificationtGrp);

    m_autoDownloadUpdateTips->setWordWrap(true);
    m_autoDownloadUpdateTips->setAlignment(Qt::AlignLeft);
    m_autoDownloadUpdateTips->setContentsMargins(10, 0, 10, 0);
    contentLayout->addWidget(m_autoDownloadUpdateTips);
    contentLayout->addSpacing(20);

#if 0 // 定时、闲时下载功能需添加时再显示
    //自动下载更新控件初始化
    m_timerDownload = new SwitchWidget("定时下载可用更新", this);
    m_timerDownloadLbl = new DTipLabel("当前设置时间为：", this);
    m_timerDownloadLbl->setWordWrap(true);
    m_timerDownloadLbl->setAlignment(Qt::AlignLeft);
    m_timerDownloadLbl->setContentsMargins(10, 0, 10, 0);
    m_setTimerLbl = new QLabel(QString("<a style='color: blue; text-decoration: none;'; href=' '>%1</a>").arg("更改"));

    //闲时下载更新控件初始化
    m_freeTimeDownload = new SwitchWidget("闲时下载更新", this);
    m_freeTimeDownloadLbl = new DTipLabel("当前使用时间段：", this);
    m_freeTimeDownloadLbl->setWordWrap(true);
    m_freeTimeDownloadLbl->setAlignment(Qt::AlignLeft);
    m_freeTimeDownloadLbl->setContentsMargins(10, 0, 10, 0);
    m_setFreeTimeLbl = new QLabel(QString("<a style='color: blue; text-decoration: none;'; href=' '>%1</a>").arg("更改"));

    auto setDownloadTimeCtrlLayout = [ & ](SwitchWidget * setSwitch, DTipLabel * timeInfoLbl, QLabel * changeLbl) {
        setSwitch->addBackground();
        contentLayout->addWidget(setSwitch);
        QHBoxLayout *downloadLblLayout = new QHBoxLayout;
        downloadLblLayout->addWidget(timeInfoLbl);
        downloadLblLayout->addWidget(changeLbl);
        contentLayout->addLayout(downloadLblLayout);
    };

    //定时下载更新布局
    setDownloadTimeCtrlLayout(m_timerDownload, m_timerDownloadLbl, m_setTimerLbl);
    //闲时下载更新布局
    setDownloadTimeCtrlLayout(m_freeTimeDownload, m_freeTimeDownloadLbl, m_setFreeTimeLbl);
#endif

    //~ contents_path /update/Update Settings
    m_autoCleanCache->setTitle(tr("Clear Package Cache"));
    m_autoCleanCache->addBackground();
    contentLayout->addWidget(m_autoCleanCache);

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
    connect(m_autoCheckUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::setUpdateMode);
    connect(m_autoCheckSecureUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::setUpdateMode);
    connect(m_autoCheckSystemUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::setUpdateMode);
    connect(m_autoCheckAppUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::setUpdateMode);
    connect(m_updateNotify, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetUpdateNotify);
    connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoDownloadUpdates);
    //connect(m_setTimerLbl, &QLabel::linkActivated,);
    //connect(m_setFreeTimeLbl, &QLabel::linkActivated,);
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

void UpdateSettings::setModel(UpdateModel *model)
{
    if (m_model == model)
        return;

    m_model = model;

    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
    // connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckSecureUpdate, &SwitchWidget::setVisible);
    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckSystemUpdate, &SwitchWidget::setVisible);
    connect(model, &UpdateModel::autoCheckSecureUpdatesChanged, m_autoCheckSecureUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckSystemUpdatesChanged, m_autoCheckSystemUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckSystemUpdatesChanged, m_autoCheckAppUpdate, &SwitchWidget::setVisible);
    connect(model, &UpdateModel::autoCheckAppUpdatesChanged, m_autoCheckAppUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::updateNotifyChanged, m_updateNotify, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::updateNotifyChanged, m_autoDownloadUpdate, &SwitchWidget::setVisible);
    connect(model, &UpdateModel::updateNotifyChanged, m_autoDownloadUpdateTips, &DTipLabel::setVisible);
    connect(model, &UpdateModel::autoDownloadUpdatesChanged, m_autoDownloadUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCleanCacheChanged, m_autoCleanCache, &SwitchWidget::setChecked);

    m_autoCheckUpdate->setChecked(model->autoCheckUpdates());
    m_autoCheckSecureUpdate->setChecked(model->autoCheckSecureUpdates());
    // m_autoCheckSecureUpdate->setVisible(model->autoCheckUpdates());
    m_autoCheckSecureUpdate->setVisible(false); // 当前阶段暂时隐藏安全更新选项显示
    m_autoCheckSystemUpdate->setChecked(model->autoCheckSystemUpdates());
    m_autoCheckSystemUpdate->setVisible(model->autoCheckUpdates());
    m_autoCheckAppUpdate->setChecked(model->autoCheckAppUpdates());
    m_autoCheckAppUpdate->setVisible(model->autoCheckSystemUpdates());
    m_updateNotify->setChecked(model->updateNotify());
    m_autoDownloadUpdate->setChecked(model->autoDownloadUpdates());
    m_autoDownloadUpdate->setVisible(model->updateNotify());
    m_autoDownloadUpdateTips->setVisible(model->updateNotify());
    m_autoCleanCache->setChecked(m_model->autoCleanCache());

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (!IsServerSystem && !IsProfessionalSystem && !IsHomeSystem  && !IsEducationSystem && !IsDeepinDesktop) {
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
    if (m_autoCheckUpdate->checked()) {
        if (!m_autoCheckSystemUpdate->checked()) {
            m_autoCheckAppUpdate->setChecked(false);
        }
        updateMode = updateMode | m_autoCheckSecureUpdate->checked();
        updateMode = (updateMode << 1) | m_autoCheckAppUpdate->checked();
        updateMode = (updateMode << 1) | m_autoCheckSystemUpdate->checked();
    }
    requestSetUpdateMode(updateMode);
}
