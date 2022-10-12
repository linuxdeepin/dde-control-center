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
#pragma once

#include "interface/pagemodule.h"
#include <DTipLabel>

namespace DCC_NAMESPACE {
class SwitchWidget;
class SettingsGroup;
}

class UpdateModel;
class UpdateWorker;
class UpdateSettingsModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    enum class SettingsMethod {
        Init = -1, // 初始化状态，用来对数据进行初始化
        autoDownload = 0,  // 自动下载 =》 自动安装开关
    };

public:
    explicit UpdateSettingsModule(UpdateModel *model, UpdateWorker *work, QObject *parent = nullptr);
    virtual ~UpdateSettingsModule();
    void deactive() override;

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

private Q_SLOTS:
    void setUpdateMode();
    void setCheckStatus(QWidget *widget, bool state, const QString &key);
    void onAutoUpdateCheckChanged();
    void onAutoSecureUpdateCheckChanged();

private:
    void initConnection();
    QString getAutoInstallUpdateType(quint64 type);
    /**
     * @brief setAutoCheckEnable  更新全部关闭 显示关闭
     */
    void setAutoCheckEnable();

private:
    UpdateModel *m_model;
    UpdateWorker *m_work;

    DCC_NAMESPACE::SwitchWidget *m_autoCheckUniontechUpdate; // 检查系统更新
    DCC_NAMESPACE::SwitchWidget *m_autoCheckSecureUpdate; // 检查安全更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoCheckSecureUpdateTips;
    DCC_NAMESPACE::SwitchWidget *m_autoCheckThirdpartyUpdate; // 第三方仓库更新

    DCC_NAMESPACE::SwitchWidget *m_autoCheckUpdate;       // 自动检查
    DCC_NAMESPACE::SwitchWidget *m_autoDownloadUpdate;    // 自动下载
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoDownloadUpdateTips;

    DCC_NAMESPACE::SwitchWidget *m_autoInstallUpdate;     // 自动安装
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoInstallUpdatesTips;

    DCC_NAMESPACE::SwitchWidget *m_updateNotify;          // 更新提醒
    DCC_NAMESPACE::SwitchWidget *m_autoCleanCache; // 清除软件包

    DCC_NAMESPACE::ModuleObject *m_autoInstallUpdateModule;
    DCC_NAMESPACE::ModuleObject *m_autoInstallUpdatesTipsModule;
};
