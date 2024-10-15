// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatesettingsmodule.h"

#include "common.h"
#include "dsysinfo.h"
#include "interface/moduleobject.h"
#include "itemmodule.h"
#include "mirrorswidget.h"
#include "updatemodel.h"
#include "updateplugin.h"
#include "updatework.h"
#include "widgets/internalbuttonitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "widgets/widgetmodule.h"

#include <DFontSizeManager>
#include <DTipLabel>

#include <QVBoxLayout>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

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

UpdateSettingsModule::~UpdateSettingsModule() { }

void UpdateSettingsModule::active()
{
    setAutoCheckEnable(m_model->autoCheckSecureUpdates() || m_model->getAutoCheckThirdpartyUpdates()
                       || m_model->autoCheckSystemUpdates());
}

void UpdateSettingsModule::initModuleList()
{
    // 可从仓库更新
    appendChild(new UpdateTitleModule("updateSettings", tr("Update Settings")));
    appendChild(new WidgetModule<SwitchWidget>(
            "updateSystemUpdate",
            tr("System"),
            [this](SwitchWidget *systemSwitch) {
                m_autoCheckUniontechUpdate = systemSwitch;
                connect(m_model,
                        &UpdateModel::autoCheckSystemUpdatesChanged,
                        m_autoCheckUniontechUpdate,
                        [=](const bool status) {
                            m_autoCheckUniontechUpdate->setChecked(status);
                            setAutoCheckEnable(m_model->autoCheckSecureUpdates()
                                               || m_model->getAutoCheckThirdpartyUpdates()
                                               || m_autoCheckUniontechUpdate->checked());
                        });
                connect(m_autoCheckUniontechUpdate,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::onAutoUpdateCheckChanged);

                m_autoCheckUniontechUpdate->setTitle(tr("System"));
                m_autoCheckUniontechUpdate->addBackground();
                m_autoCheckUniontechUpdate->setChecked(m_model->autoCheckSystemUpdates());
            }));

    if (IsProfessionalSystem) {
        appendChild(new WidgetModule<SwitchWidget>(
                "securityUpdatesOnly",
                tr("Security Updates Only"),
                [this](SwitchWidget *systemSwitch) {
                    m_autoCheckSecureUpdate = systemSwitch;
                    connect(m_model,
                            &UpdateModel::autoCheckSecureUpdatesChanged,
                            m_autoCheckSecureUpdate,
                            [=](const bool status) {
                                m_autoCheckSecureUpdate->setChecked(status);
                                setAutoCheckEnable(m_autoCheckSecureUpdate->checked()
                                                   || m_autoCheckUniontechUpdate->checked());
                            });
                    connect(m_autoCheckSecureUpdate,
                            &SwitchWidget::checkedChanged,
                            this,
                            &UpdateSettingsModule::onAutoSecureUpdateCheckChanged);

                    m_autoCheckSecureUpdate->setTitle(tr("Security Updates Only"));
                    m_autoCheckSecureUpdate->addBackground();
                    m_autoCheckSecureUpdate->setChecked(m_model->autoCheckSecureUpdates());
                }));
        appendChild(new WidgetModule<DTipLabel>(
                "securityUpdatesOnlyTips",
                tr("Switch it on to only update security vulnerabilities and compatibility issues"),
                [this](DTipLabel *systemLabel) {
                    m_autoCheckSecureUpdateTips = systemLabel;
                    m_autoCheckSecureUpdateTips->setWordWrap(true);
                    m_autoCheckSecureUpdateTips->setAlignment(Qt::AlignLeft);
                    m_autoCheckSecureUpdateTips->setContentsMargins(10, 0, 10, 0);
                    m_autoCheckSecureUpdateTips->setText(
                            tr("Switch it on to only update security vulnerabilities and "
                               "compatibility issues"));
                }));
    }

    if (IsCommunitySystem) {
        appendChild(new WidgetModule<SwitchWidget>(
                "thirdpartyRepositories",
                tr("Third-party Repositories"),
                [this](SwitchWidget *systemSwitch) {
                    m_autoCheckThirdpartyUpdate = systemSwitch;
                    connect(m_model,
                            &UpdateModel::autoCheckThirdpartyUpdatesChanged,
                            m_autoCheckThirdpartyUpdate,
                            [=](const bool status) {
                                m_autoCheckThirdpartyUpdate->setChecked(status);
                                setAutoCheckEnable(m_autoCheckThirdpartyUpdate->checked()
                                                   || m_autoCheckUniontechUpdate->checked());
                            });
                    connect(m_autoCheckThirdpartyUpdate,
                            &SwitchWidget::checkedChanged,
                            this,
                            &UpdateSettingsModule::onAutoUpdateCheckChanged);

                    m_autoCheckThirdpartyUpdate->setTitle(tr("Third-party Repositories"));
                    m_autoCheckThirdpartyUpdate->addBackground();
                    m_autoCheckThirdpartyUpdate->setChecked(
                            m_model->getAutoCheckThirdpartyUpdates());
                }));
    }
    // 其他设置
    appendChild(new UpdateTitleModule("otherSettings", tr("Other settings")));
    m_autoCheckUpdateModule = new WidgetModule<SwitchWidget>(
            "updateAutoCheck",
            tr("Auto Check for Updates"),
            [this](SwitchWidget *systemSwitch) {
                m_autoCheckUpdate = systemSwitch;
                connect(m_model,
                        &UpdateModel::autoCheckUpdatesChanged,
                        m_autoCheckUpdate,
                        &SwitchWidget::setChecked);
                connect(m_autoCheckUpdate,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::requestSetAutoCheckUpdates);

                m_autoCheckUpdate->setTitle(tr("Auto Check for Updates"));
                m_autoCheckUpdate->addBackground();
                m_autoCheckUpdate->setChecked(m_model->autoCheckUpdates());
            });
    appendChild(m_autoCheckUpdateModule);

    m_autoDownloadUpdateModule = new WidgetModule<SwitchWidget>(
            "updateAutoDownlaod",
            tr("Auto Download Updates"),
            [this](SwitchWidget *systemSwitch) {
                m_autoDownloadUpdate = systemSwitch;
                connect(m_model,
                        &UpdateModel::autoDownloadUpdatesChanged,
                        m_autoDownloadUpdate,
                        &SwitchWidget::setChecked);
                connect(m_autoDownloadUpdate,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::requestSetAutoDownloadUpdates);
                connect(m_autoDownloadUpdate, &SwitchWidget::checkedChanged, this, [=](bool state) {
                    uiMethodChanged(state ? SettingsMethod::Init : SettingsMethod::autoDownload);
                });

                m_autoDownloadUpdate->setTitle(tr("Auto Download Updates"));
                m_autoDownloadUpdate->addBackground();
                m_autoDownloadUpdate->setChecked(m_model->autoDownloadUpdates());
            });
    appendChild(m_autoDownloadUpdateModule);

    m_autoDownloadUpdateTipsModule = new WidgetModule<
            DTipLabel>("updateAutoDownlaodTips", tr(""), [this](DTipLabel *systemLabel) {
        m_autoDownloadUpdateTips = systemLabel;
        m_autoDownloadUpdateTips->setWordWrap(true);
        m_autoDownloadUpdateTips->setAlignment(Qt::AlignLeft);
        m_autoDownloadUpdateTips->setContentsMargins(10, 0, 10, 0);
        m_autoDownloadUpdateTips->setText(tr(
                "Switch it on to automatically download the updates in wireless or wired network"));
    });
    appendChild(m_autoDownloadUpdateTipsModule);

    m_autoInstallUpdateModule = new WidgetModule<SwitchWidget>(
            "autoInstallUpdates",
            tr("Auto Install Updates"),
            [this](SwitchWidget *systemSwitch) {
                m_autoInstallUpdate = systemSwitch;
                connect(m_model,
                        &UpdateModel::autoInstallUpdatesChanged,
                        m_autoInstallUpdate,
                        &SwitchWidget::setChecked);
                connect(m_autoInstallUpdate,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::requestSetAutoInstall);
                m_autoInstallUpdate->setTitle(tr("Auto Install Updates"));
                m_autoInstallUpdate->addBackground();
                m_autoInstallUpdate->setChecked(m_model->getAutoInstallUpdates());
            });
    appendChild(m_autoInstallUpdateModule);
    m_autoInstallUpdatesTipsModule = new WidgetModule<DTipLabel>(
            "autoInstallUpdatesTips",
            tr(""),
            [this](DTipLabel *systemLabel) {
                m_autoInstallUpdatesTips = systemLabel;
                connect(m_model,
                        &UpdateModel::autoInstallUpdateTypeChanged,
                        this,
                        [=](quint64 type) {
                            m_autoInstallUpdatesTips->setText(getAutoInstallUpdateType(type));
                        });
                m_autoInstallUpdatesTips->setWordWrap(true);
                m_autoInstallUpdatesTips->setAlignment(Qt::AlignLeft);
                m_autoInstallUpdatesTips->setContentsMargins(10, 0, 10, 0);
                m_autoInstallUpdatesTips->setText(
                        getAutoInstallUpdateType(m_model->getAutoInstallUpdateType()));
            });
    appendChild(m_autoInstallUpdatesTipsModule);
    m_backupUpdatesModule = new WidgetModule<SwitchWidget>(
            "backupUpdates",
            tr("Backup updates"),
            [this](SwitchWidget *systemSwitch) {
                m_backupUpdates = systemSwitch;
                connect(m_model,
                        &UpdateModel::backupUpdatesChanged,
                        m_backupUpdates,
                        &SwitchWidget::setChecked);
                connect(m_backupUpdates,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::requestSetBackupUpdates);
                m_backupUpdates->setTitle(tr("Backup updates"));
                m_backupUpdates->addBackground();
                m_backupUpdates->setChecked(m_model->getBackupUpdates());
            });
    appendChild(m_backupUpdatesModule);

    m_backupUpdatesTipModule = new WidgetModule<DTipLabel>(
            "backupsUpdateTip",
            tr(""),
            [this](DTipLabel *systemLabel) {
                m_backupUpdatesTip = systemLabel;
                m_backupUpdatesTip->setWordWrap(true);
                m_backupUpdatesTip->setAlignment(Qt::AlignLeft);
                m_backupUpdatesTip->setContentsMargins(10, 0, 10, 0);
                m_backupUpdatesTip->setText(tr("Ensuring normal system rollback in case of upgrade failure"));
            });
    appendChild(m_backupUpdatesTipModule);

    m_updateNotifyModule = new WidgetModule<SwitchWidget>(
            "updateUpdateNotify",
            tr("Updates Notification"),
            [this](SwitchWidget *systemSwitch) {
                m_updateNotify = systemSwitch;
                connect(m_model,
                        &UpdateModel::updateNotifyChanged,
                        m_updateNotify,
                        &SwitchWidget::setChecked);
                connect(m_updateNotify,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::requestSetUpdateNotify);

                m_updateNotify->setTitle(tr("Updates Notification"));
                m_updateNotify->addBackground();
                m_updateNotify->setChecked(m_model->updateNotify());
            });
    appendChild(m_updateNotifyModule);

    appendChild(new WidgetModule<SwitchWidget>(
            "updateCleanCache",
            tr("Clear Package Cache"),
            [this](SwitchWidget *systemSwitch) {
                m_autoCleanCache = systemSwitch;
                connect(m_model,
                        &UpdateModel::autoCleanCacheChanged,
                        m_autoCleanCache,
                        &SwitchWidget::setChecked);
                connect(m_autoCleanCache,
                        &SwitchWidget::checkedChanged,
                        this,
                        &UpdateSettingsModule::requestSetAutoCleanCache);

                m_autoCleanCache->setTitle(tr("Clear Package Cache"));
                m_autoCleanCache->addBackground();
                m_autoCleanCache->setChecked(m_model->autoCleanCache());
            }));

    if (IsCommunitySystem) {
        // 智能镜像源
        appendChild(new WidgetModule<SwitchWidget>("smartMirrorSwitch", tr("Smart Mirror Switch"), [this](SwitchWidget *smartMirrorBtn) {
            m_smartMirrorBtn = smartMirrorBtn;
            connect(m_model, &UpdateModel::smartMirrorSwitchChanged, smartMirrorBtn, [smartMirrorBtn](const bool smartMirrorSwitch) {
                smartMirrorBtn->setChecked(smartMirrorSwitch);
            });
            connect(smartMirrorBtn, &SwitchWidget::checkedChanged, this, [this](const bool checked) {
                m_work->setSmartMirror(checked);
            });

            smartMirrorBtn->setTitle(tr("Smart Mirror Switch"));
            smartMirrorBtn->addBackground();
            smartMirrorBtn->setChecked(m_model->smartMirrorSwitch());
        }));
        appendChild(new ItemModule(
                "smartTips",
                tr("Switch it on to connect to the quickest mirror site automatically"),
                [](ModuleObject *object) {
                    DTipLabel *label = new DTipLabel();
                    label->setWordWrap(true);
                    label->setAlignment(Qt::AlignLeft);
                    label->setContentsMargins(10, 0, 10, 0);
                    label->setText(object->displayName());
                    return label;
                },
                false));
        auto updateMirrors = new ItemModule("mirrorList", tr("Mirror List"), [this](ModuleObject *) {
            QWidget *widget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->setMargin(0);
            layout->addStretch();

            QLabel *mirrors = new QLabel(widget);
            layout->addWidget(mirrors);
            mirrors->setText(m_model->defaultMirror().m_name);
            connect(m_model, &UpdateModel::defaultMirrorChanged, mirrors, [mirrors](const MirrorInfo& mirror){
                mirrors->setText(mirror.m_name);
            });
            QLabel *enterIcon = new QLabel(widget);
            enterIcon->setPixmap(DStyle::standardIcon(widget->style(), DStyle::SP_ArrowEnter).pixmap(16, 16));
            layout->addWidget(enterIcon);
            return widget;
        });
        updateMirrors->setBackground(true);
        updateMirrors->setClickable(true);
        appendChild(updateMirrors);
        connect(m_model, &UpdateModel::smartMirrorSwitchChanged, updateMirrors, [updateMirrors](const bool smartMirrorSwitch) {
            updateMirrors->setVisible(!smartMirrorSwitch);
        });
        updateMirrors->setVisible(!m_model->smartMirrorSwitch());
        connect(updateMirrors, &ItemModule::clicked, this, [this]() {
            if (m_mirrorsWidget.isNull()) {
                m_mirrorsWidget.reset(new MirrorsWidget(m_model));
                m_mirrorsWidget->setWindowModality(Qt::ApplicationModal);
                m_mirrorsWidget->setVisible(false);
                m_mirrorsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

                connect(m_mirrorsWidget.get(), &MirrorsWidget::requestSetDefaultMirror, m_work, &UpdateWorker::setMirrorSource);
                connect(m_mirrorsWidget.get(), &MirrorsWidget::requestTestMirrorSpeed, m_work, &UpdateWorker::testMirrorSpeed);
            }
            m_work->checkNetselect();
            m_mirrorsWidget->show();
        });

        appendChild(new UpdateTitleModule("InternalUpdateSetting",
                                          tr("Updates from Internal Testing Sources")));
        appendChild(new WidgetModule<InternalButtonItem>(
                "internal update",
                tr("internal update"),
                [this](InternalButtonItem *internalBtn) {
                    internalBtn->addBackground();
                    internalBtn->onModelTestingStatusChanged(m_model->getTestingChannelStatus());
                    connect(internalBtn,
                            &InternalButtonItem::requestInternalChannel,
                            this,
                            [this, internalBtn](bool enable) {
                                if (enable) {
                                    auto url = m_work->getTestingChannelUrl();
                                    if (url.has_value()) {
                                        internalBtn->setLink(url.value());
                                        m_work->setTestingChannelEnable(true);
                                        return;
                                    }
                                }
                                m_work->setTestingChannelEnable(false);
                            });
                    connect(m_model,
                            &UpdateModel::TestingChannelStatusChanged,
                            internalBtn,
                            &InternalButtonItem::onModelTestingStatusChanged);
                }));
        auto internalUpdateTip = new WidgetModule<DTipLabel>(
                "internalUpdateTip",
                tr(""),
                [](DTipLabel *internalUpdateLabel) {
                    internalUpdateLabel->setWordWrap(true);
                    internalUpdateLabel->setAlignment(Qt::AlignLeft);
                    internalUpdateLabel->setContentsMargins(10, 0, 10, 0);
                    internalUpdateLabel->setText(tr("Join the internal testing channel to get deepin latest updates"));
                });
        appendChild(internalUpdateTip);
    }
}

void UpdateSettingsModule::uiMethodChanged(SettingsMethod uiMethod)
{
    // v23自动安装开关始终隐藏
    m_autoInstallUpdateModule->setHidden(true);
    m_autoInstallUpdatesTipsModule->setHidden(true);
}

void UpdateSettingsModule::initConnection()
{
    connect(this,
            &UpdateSettingsModule::requestSetUpdateMode,
            m_work,
            &UpdateWorker::setUpdateMode);

    connect(this,
            &UpdateSettingsModule::requestSetAutoCheckUpdates,
            m_work,
            &UpdateWorker::setAutoCheckUpdates);
    connect(this,
            &UpdateSettingsModule::requestSetAutoDownloadUpdates,
            m_work,
            &UpdateWorker::setAutoDownloadUpdates);
    connect(this,
            &UpdateSettingsModule::requestSetAutoInstall,
            m_work,
            &UpdateWorker::setAutoInstallUpdates);
    connect(this,
            &UpdateSettingsModule::requestSetBackupUpdates,
            m_work,
            &UpdateWorker::setBackupUpdates);
    connect(this,
            &UpdateSettingsModule::requestSetUpdateNotify,
            m_work,
            &UpdateWorker::setUpdateNotify);

    connect(this,
            &UpdateSettingsModule::requestSetAutoCleanCache,
            m_work,
            &UpdateWorker::setAutoCleanCache);
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
    auto setCheckEnable = [=](DCC_NAMESPACE::ModuleObject *widgetModule) {
        widgetModule->setEnabled(checkstatus);
    };

    setCheckEnable(m_autoCheckUpdateModule);
    setCheckEnable(m_autoDownloadUpdateModule);
    setCheckEnable(m_autoDownloadUpdateTipsModule);
    setCheckEnable(m_autoInstallUpdateModule);
    setCheckEnable(m_autoInstallUpdatesTipsModule);
    setCheckEnable(m_backupUpdatesModule);
    setCheckEnable(m_backupUpdatesTipModule);
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

    setAutoCheckEnable(m_model->autoCheckSecureUpdates() || m_model->getAutoCheckThirdpartyUpdates()
                       || m_model->autoCheckSystemUpdates());
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
