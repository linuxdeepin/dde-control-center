// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
#include <DDialog>
#include <DWaterProgress>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QGSettings>
#include <QMessageBox>

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
    , m_autoInstallUpdatesTips(new DTipLabel(tr("")))
    , m_autoDownloadUpdateTips(new DTipLabel(tr("Switch it on to automatically download the updates in wireless or wired network"), this))
    , m_autoCheckSecureUpdateTips(new DTipLabel(tr("Switch it on to only update security vulnerabilities and compatibility issues"), this))
    , m_testingChannelTips(new DTipLabel(tr("Join the internal testing channel to get deepin latest updates")))
    , m_testingChannelLinkLabel(new QLabel(""))
    , m_autoCleanCache(new SwitchWidget(this))
    , m_dconfig(nullptr)
{
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoCheckSecureUpdate = new SwitchWidget(tr("Security Updates Only"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoCheckUniontechUpdate = new SwitchWidget(tr("System"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoCheckAppUpdate = new SwitchWidget(tr("App installed in App Store"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoCheckThirdpartyUpdate = new SwitchWidget(tr("Third-party Repositories"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_updateNotify = new SwitchWidget(tr("Updates Notification"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoDownloadUpdate = new SwitchWidget(tr("Auto Download Updates"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoInstallUpdate = new SwitchWidget(tr("Auto Install Updates"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_testingChannel = new SwitchWidget(tr("Join Internal Testing Channel"), this);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_testingChannelHeadingLabel = new QLabel(tr("Updates from Internal Testing Sources"));

    initUi();
    initConnection();
    setModel(model);

    if (IsCommunitySystem) {
        m_autoCheckUniontechUpdate->setTitle(tr("System"));
        m_autoCheckSecureUpdate->hide();
        m_autoCheckSecureUpdateTips->hide();
    }
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

    contentLayout->addSpacing(20);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    QLabel *autoUpdateSettingsLabel = new QLabel(tr("Updates from Repositories"), this);
    DFontSizeManager::instance()->bind(autoUpdateSettingsLabel, DFontSizeManager::T5, QFont::DemiBold);
    autoUpdateSettingsLabel->setContentsMargins(10, 0, 10, 0); // 左右边距为10
    contentLayout->addWidget(autoUpdateSettingsLabel);
    contentLayout->addSpacing(10);

    SettingsGroup *updatesGrp = new SettingsGroup(contentWidget);
    updatesGrp->appendItem(m_autoCheckUniontechUpdate);
    updatesGrp->appendItem(m_autoCheckAppUpdate);
    updatesGrp->appendItem(m_autoCheckSecureUpdate);
    contentLayout->addWidget(updatesGrp);
    m_autoCheckSecureUpdateTips->setWordWrap(true);
    m_autoCheckSecureUpdateTips->setAlignment(Qt::AlignLeft);
    m_autoCheckSecureUpdateTips->setContentsMargins(10, 0, 10, 0);
    contentLayout->addWidget(m_autoCheckSecureUpdateTips);
    contentLayout->addSpacing(10);
    SettingsGroup *autoCheckThirdpartyGrp = new SettingsGroup(contentWidget);
    autoCheckThirdpartyGrp->appendItem(m_autoCheckThirdpartyUpdate);
    contentLayout->addWidget(autoCheckThirdpartyGrp);

    contentLayout->addSpacing(20);
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    QLabel *otherSettingsLabel = new QLabel(tr("Other settings"), this);
    DFontSizeManager::instance()->bind(otherSettingsLabel, DFontSizeManager::T5, QFont::DemiBold);
    otherSettingsLabel->setContentsMargins(10, 0, 10, 0); // 左右边距为10
    contentLayout->addWidget(otherSettingsLabel);
    contentLayout->addSpacing(10);

    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoCheckUpdate->setTitle(tr("Auto Check for Updates"));
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
    //~ contents_path /update/Update Settings
    //~ child_page Update Settings
    m_autoCleanCache->setTitle(tr("Clear Package Cache"));
    updatesNotificationtGrp->appendItem(m_autoCleanCache);
    contentLayout->addWidget(updatesNotificationtGrp);

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    if (!IsServerSystem && !IsProfessionalSystem && !IsHomeSystem && !IsEducationSystem && !IsDeepinDesktop) {
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
        m_smartMirrorBtn = new SwitchWidget(tr("Smart Mirror Switch"), this);
        m_smartMirrorBtn->addBackground();
        contentLayout->addWidget(m_smartMirrorBtn);

        DTipLabel *smartTips = new DTipLabel(tr("Switch it on to connect to the quickest mirror site automatically"), this);
        smartTips->setWordWrap(true);
        smartTips->setAlignment(Qt::AlignLeft);
        smartTips->setContentsMargins(10, 0, 10, 0);
        contentLayout->addWidget(smartTips);

        m_updateMirrors = new NextPageWidget(nullptr, false);
        m_updateMirrors->setTitle(tr("Mirror List"));
        m_updateMirrors->setRightTxtWordWrap(true);
        m_updateMirrors->addBackground();
        QStyleOption opt;
        m_updateMirrors->setBtnHiden(true);
        m_updateMirrors->setIconIcon(DStyleHelper(m_updateMirrors->style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr).pixmap(10, 10));
        contentLayout->addWidget(m_updateMirrors);
        contentLayout->addSpacing(20);

        DFontSizeManager::instance()->bind(m_testingChannelHeadingLabel, DFontSizeManager::T5, QFont::DemiBold);
        m_testingChannelHeadingLabel->setContentsMargins(10, 0, 10, 0); // 左右边距为10
        contentLayout->addWidget(m_testingChannelHeadingLabel);
        contentLayout->addSpacing(10);
        // Add link label to switch button
        m_testingChannelLinkLabel->setOpenExternalLinks(true);
        m_testingChannelLinkLabel->setStyleSheet("font: 12px");
        auto mainLayout = m_testingChannel->getMainLayout();
        mainLayout->insertWidget(mainLayout->count()-1, m_testingChannelLinkLabel);

        m_testingChannel->addBackground();
        contentLayout->addWidget(m_testingChannel);
        m_testingChannelTips->setWordWrap(true);
        m_testingChannelTips->setAlignment(Qt::AlignLeft);
        m_testingChannelTips->setContentsMargins(10, 0, 10, 0);
        contentLayout->addWidget(m_testingChannelTips);
        m_testingChannel->setVisible(false);
        m_testingChannelTips->setVisible(false);
    } else {
        m_testingChannel->hide();
        m_testingChannelTips->hide();
        m_testingChannelLinkLabel->hide();
        m_testingChannelHeadingLabel->hide();
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
    connect(m_autoCheckThirdpartyUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::onAutoUpdateCheckChanged);
    connect(m_updateNotify, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetUpdateNotify);
    connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, &UpdateSettings::requestSetAutoDownloadUpdates);
    connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, [ = ](bool state) {
        setCheckStatus(m_autoInstallUpdate, state, "updateAutoInstall");
        setCheckStatus(m_autoInstallUpdatesTips, state, "updateAutoInstall");
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

        connect(m_testingChannel, &SwitchWidget::checkedChanged, this, &UpdateSettings::onTestingChannelCheckChanged);
    }
}

QString UpdateSettings::getAutoInstallUpdateType(quint64 type)
{
    QString text = "";
    if (type & ClassifyUpdateType::SystemUpdate) {
        //~ contents_path /update/Update Settings
        //~ child_page General
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

void UpdateSettings::setAutoCheckEnable(bool enable)
{
    auto setCheckEnable = [ = ](QWidget * widget, bool state, const QString & key, bool useDconfig) {
        QString status = DConfigWatcher::instance()->getStatus(DConfigWatcher::ModuleType::update, key);
        if (!useDconfig) {
            status = GSettingWatcher::instance()->get(key).toString();
        }

        widget->setEnabled("Enabled" == status && enable);
    };

    setCheckEnable(m_autoCheckUpdate, enable, "updateAutoCheck", false);
    setCheckEnable(m_autoDownloadUpdate, enable, "updateAutoDownlaod", false);
    setCheckEnable(m_autoDownloadUpdateTips, enable, "updateAutoDownlaod", false);
    setCheckEnable(m_updateNotify, enable, "updateUpdateNotify", false);
    setCheckEnable(m_autoInstallUpdate, enable, "updateAutoInstall", true);
    setCheckEnable(m_autoInstallUpdatesTips, enable, "updateAutoInstall", true);
}

void UpdateSettings::setModel(UpdateModel *model)
{
    if (m_model == model)
        return;

    m_model = model;

    connect(model, &UpdateModel::autoCheckUpdatesChanged, m_autoCheckUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckSecureUpdatesChanged, m_autoCheckSecureUpdate, [ = ](const bool status) {
        m_autoCheckSecureUpdate->setChecked(status);
        setAutoCheckEnable(m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());
    });
    connect(model, &UpdateModel::autoCheckSystemUpdatesChanged, m_autoCheckUniontechUpdate, [ = ](const bool status) {
        m_autoCheckUniontechUpdate->setChecked(status);
        setAutoCheckEnable(m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());
    });
    connect(model, &UpdateModel::autoCheckAppUpdatesChanged, m_autoCheckAppUpdate, &SwitchWidget::setChecked);
    connect(model, &UpdateModel::autoCheckThirdpartyUpdatesChanged, m_autoCheckThirdpartyUpdate, [ = ](const bool status) {
        m_autoCheckThirdpartyUpdate->setChecked(status);
        setAutoCheckEnable(m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());
    });
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
    m_autoCheckThirdpartyUpdate->setChecked(model->getAutoCheckThirdpartyUpdates());
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
    if (m_dconfig && m_dconfig->isValid()) {
        connect(m_dconfig, &DConfig::valueChanged, this, [ = ](const QString& key) {
            if (key == "updateAutoInstall") {
                setCheckStatus(m_autoInstallUpdate, m_autoDownloadUpdate->checked(), "updateAutoInstall");
                setCheckStatus(m_autoInstallUpdatesTips, m_autoDownloadUpdate->checked(), "updateAutoInstall");
            }
        });
    }
    DConfigWatcher::instance()->bind(DConfigWatcher::update, "updateSafety", m_autoCheckSecureUpdate);
    DConfigWatcher::instance()->bind(DConfigWatcher::update, "updateSafety", m_autoCheckSecureUpdateTips);
    if (DCC_NAMESPACE::IsProfessionalSystem) {
        DConfigWatcher::instance()->bind(DConfigWatcher::update, "updateThirdPartySource", m_autoCheckThirdpartyUpdate);
    }

    connect(GSettingWatcher::instance(), &GSettingWatcher::notifyGSettingsChanged, this, [ = ](const QString & gsetting, const QString & state) {
        bool status = GSettingWatcher::instance()->get(gsetting).toString() == "Enabled" && (m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());
        if (gsetting == "updateAutoCheck") {
            m_autoCheckUpdate->setEnabled(status);
        }

        if (gsetting == "updateAutoDownlaod") {
            m_autoDownloadUpdate->setEnabled(status);
            m_autoDownloadUpdateTips->setEnabled(status);
        }

        if (gsetting == "updateUpdateNotify") {
            m_updateNotify->setEnabled(status);
        }
    });
    setAutoCheckEnable(m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());

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

        auto hyperLink = QString("<a href='%1'>%2</a>").arg(m_model->getTestingChannelJoinURL().toString(),tr("here"));
        m_testingChannelLinkLabel->setText(tr("Click %1 to complete the application").arg(hyperLink));
        connect(model, &UpdateModel::testingChannelStatusChanged, this, &UpdateSettings::onTestingChannelStatusChanged);
        onTestingChannelStatusChanged();
    }
}

void UpdateSettings::onTestingChannelStatusChanged()
{
    const auto channelStatus = m_model->getTestingChannelStatus();
    if (channelStatus == UpdateModel::TestingChannelStatus::Hidden) {
        m_testingChannelHeadingLabel->hide();
        m_testingChannel->hide();
        m_testingChannelTips->hide();
        m_testingChannelLinkLabel->hide();
    } else {
        m_testingChannelHeadingLabel->show();
        m_testingChannel->show();
        m_testingChannelTips->show();
        m_testingChannelLinkLabel->setVisible(channelStatus == UpdateModel::TestingChannelStatus::WaitJoined);
        m_testingChannel->setChecked(channelStatus != UpdateModel::TestingChannelStatus::NotJoined);
    }
}

void UpdateSettings::onTestingChannelCheckChanged(const bool checked)
{
    const auto status = m_model->getTestingChannelStatus();
    if (checked) {
        Q_EMIT requestSetTestingChannelEnable(checked);
        return;
    }
    if (status != UpdateModel::TestingChannelStatus::Joined) {
        Q_EMIT requestSetTestingChannelEnable(checked);
        return;
    }

    auto dialog = new DDialog(this);
    dialog->setFixedWidth(400);
    dialog->setFixedHeight(280);

    auto label = new DLabel(dialog);
    label->setWordWrap(true);
    label->setText(tr("Checking system versions, please wait..."));

    auto progress = new DWaterProgress(dialog);
    progress->setFixedSize(100, 100);
    progress->setTextVisible(false);
    progress->setValue(50);
    progress->start();

    QWidget* content = new QWidget(dialog);
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0, 0, 0, 0);
    content->setLayout(layout);
    dialog->addContent(content);

    layout->addStretch();
    layout->addWidget(label, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(progress, 0, Qt::AlignHCenter);
    layout->addStretch();

    connect(m_model, &UpdateModel::canExitTestingChannelChanged, dialog, [ = ](const bool can) {
        progress->setVisible(false);
        if (!can)
        {
            Q_EMIT requestSetTestingChannelEnable(checked);
            dialog->deleteLater();
            return;
        }
        const auto text = tr("If you leave the internal testing channel now, you may not be able to get the latest bug fixes and updates. Please leave after the official version is released to keep your system stable!");
        label->setText(text);
        dialog->addButton(tr("Leave"), false, DDialog::ButtonWarning);
        dialog->addButton(tr("Cancel"), true, DDialog::ButtonRecommend);
    });
    // 检查有可能会很快完成，对话框一闪而过会给人一种操作出错的感觉
    // 延迟一秒后再执行检查，可以让人有时间看到对话框，提升用户体验
    QTimer::singleShot(1000, this,  [ this ] {
        Q_EMIT requestCheckCanExitTestingChannel();
    });

    connect(dialog, &DDialog::closed, this, [ = ]() {
        // clicked windows close button
        m_testingChannel->setChecked(true);
        dialog->deleteLater();
    });
    connect(dialog, &DDialog::buttonClicked, this, [ = ](int index, const QString &text) {
        if ( index == 0 ) {
            // clicked the leave button
            Q_EMIT requestSetTestingChannelEnable(checked);
        }else {
            // clicked the cancel button
            m_testingChannel->setChecked(true);
        }
        dialog->deleteLater();
    });
    dialog->exec();
}

void UpdateSettings::setUpdateMode()
{
    quint64 updateMode = 0;

    updateMode = updateMode | m_autoCheckSecureUpdate->checked();
    updateMode = (updateMode << 1) | m_autoCheckThirdpartyUpdate->checked();
    updateMode = (updateMode << 2) | m_autoCheckAppUpdate->checked();
    updateMode = (updateMode << 1) | m_autoCheckUniontechUpdate->checked();

    setAutoCheckEnable(m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked());
    requestSetUpdateMode(updateMode);
}

void UpdateSettings::setCheckStatus(QWidget *widget, bool state, const QString &key)
{
    //后续对应配置相关代码
    const QString status = DConfigWatcher::instance()->getStatus(DConfigWatcher::ModuleType::update, key);

    if ("Enabled" == status) {
        widget->setEnabled((m_autoCheckSecureUpdate->checked() || m_autoCheckThirdpartyUpdate->checked() || m_autoCheckUniontechUpdate->checked()));
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
