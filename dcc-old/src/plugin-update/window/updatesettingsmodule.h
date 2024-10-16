// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/pagemodule.h"
#include "mirrorswidget.h"

#include <DTipLabel>

namespace DCC_NAMESPACE {
class SwitchWidget;
class SettingsGroup;
} // namespace DCC_NAMESPACE

class UpdateModel;
class UpdateWorker;

class UpdateSettingsModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    enum class SettingsMethod {
        Init = -1,        // 初始化状态，用来对数据进行初始化
        autoDownload = 0, // 自动下载 =》 自动安装开关
    };

public:
    explicit UpdateSettingsModule(UpdateModel *model,
                                  UpdateWorker *work,
                                  QObject *parent = nullptr);
    virtual ~UpdateSettingsModule();
    void active() override;

    /**
     * @brief UpdateSettingsModule::initModuleList 初始化页面mouleObj
     */
    void initModuleList();

    /**
     * @brief uiMethodChanged 界面展示变跟
     */
    void uiMethodChanged(SettingsMethod uiMethod);

Q_SIGNALS:
    void requestSetAutoCheckUpdates(const bool autocheckUpdate);
    void requestSetUpdateMode(const quint64 updateMode);
    void requestSetAutoCheckSystemUpdates(const bool &autoUpdate);
    void requestSetAutoCheckAppUpdates(const bool &autoUpdate);
    void requestSetAutoCheckSecureUpdates(const bool &autoUpdate);
    void requestSetUpdateNotify(bool notify);
    void requestSetAutoDownloadUpdates(const bool &autoUpdate);
    void requestSetAutoCleanCache(const bool autoClean);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void requestSetSourceCheck(const bool check);
#endif
    void requestShowMirrorsView();
    void requestSetAutoInstall(const bool &autoInstall);
    void requestSetBackupUpdates(const bool &backupUpdates);

private Q_SLOTS:
    void setUpdateMode();
    void onAutoUpdateCheckChanged();
    void onAutoSecureUpdateCheckChanged();

private:
    void initConnection();
    QString getAutoInstallUpdateType(quint64 type);
    /**
     * @brief setAutoCheckEnable  更新全部关闭 显示关闭
     */
    void setAutoCheckEnable(bool checkstatus);

private:
    UpdateModel *m_model;
    UpdateWorker *m_work;

    DCC_NAMESPACE::SwitchWidget *m_autoCheckUniontechUpdate; // 检查系统更新
    DCC_NAMESPACE::SwitchWidget *m_autoCheckSecureUpdate;    // 检查安全更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoCheckSecureUpdateTips;
    DCC_NAMESPACE::SwitchWidget *m_autoCheckThirdpartyUpdate; // 第三方仓库更新

    DCC_NAMESPACE::SwitchWidget *m_autoCheckUpdate;    // 自动检查
    DCC_NAMESPACE::SwitchWidget *m_autoDownloadUpdate; // 自动下载
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoDownloadUpdateTips;

    DCC_NAMESPACE::SwitchWidget *m_autoInstallUpdate; // 自动安装
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoInstallUpdatesTips;

    DCC_NAMESPACE::SwitchWidget *m_backupUpdates; // 备份更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_backupUpdatesTip;

    DCC_NAMESPACE::SwitchWidget *m_updateNotify;   // 更新提醒
    DCC_NAMESPACE::SwitchWidget *m_autoCleanCache; // 清除软件包
    DCC_NAMESPACE::SwitchWidget *m_smartMirrorBtn; // 智能镜像源

    DCC_NAMESPACE::ModuleObject *m_autoCheckUpdateModule;
    DCC_NAMESPACE::ModuleObject *m_autoDownloadUpdateModule;
    DCC_NAMESPACE::ModuleObject *m_autoDownloadUpdateTipsModule;
    DCC_NAMESPACE::ModuleObject *m_autoInstallUpdateModule;
    DCC_NAMESPACE::ModuleObject *m_autoInstallUpdatesTipsModule;
    DCC_NAMESPACE::ModuleObject *m_backupUpdatesModule;
    DCC_NAMESPACE::ModuleObject *m_backupUpdatesTipModule;
    DCC_NAMESPACE::ModuleObject *m_updateNotifyModule;
    QScopedPointer<DCC_NAMESPACE::update::MirrorsWidget> m_mirrorsWidget;
};
