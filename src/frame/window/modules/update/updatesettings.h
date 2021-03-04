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

#include "interface/moduleinterface.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace widgets {
class SettingsGroup;
class SwitchWidget;
class NextPageWidget;
class TipsLabel;
}

namespace update {
class UpdateModel;
}
}

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
DWIDGET_END_NAMESPACE

const int TipLeftInterver = 10;
const int GroupSpace = 10;

namespace DCC_NAMESPACE {
namespace update {

class UpdateSettings : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateSettings(dcc::update::UpdateModel *model, QWidget *parent = 0);
    virtual ~UpdateSettings();

    void setModel(dcc::update::UpdateModel *model);

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
    void requestEnableSmartMirror(bool enable);
    void requestShowMirrorsView();

private Q_SLOTS:
    void setUpdateMode();

private:
    void initUi();
    void initConnection();

private:
    dcc::update::UpdateModel *m_model;

    dcc::widgets::SwitchWidget *m_autoCheckUpdate;       // 自动检测
    dcc::widgets::SwitchWidget *m_autoCheckSecureUpdate; // 安全性更新
    dcc::widgets::SwitchWidget *m_autoCheckSystemUpdate; // 统信UOS
    dcc::widgets::SwitchWidget *m_autoCheckAppUpdate;    // 应用商店的应用
    dcc::widgets::SwitchWidget *m_updateNotify;          // 更新提醒
    dcc::widgets::SwitchWidget *m_autoDownloadUpdate;    // 自动下载
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoDownloadUpdateTips;

    dcc::widgets::SwitchWidget *m_timerDownload; // 定时下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_timerDownloadLbl;
    QLabel *m_setTimerLbl;

    dcc::widgets::SwitchWidget *m_freeTimeDownload; // 闲时下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_freeTimeDownloadLbl;
    QLabel *m_setFreeTimeLbl;

    dcc::widgets::SwitchWidget *m_autoCleanCache; // 自动清除缓存

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    dcc::widgets::SwitchWidget *m_sourceCheck; // System Repository Detection
#endif
    dcc::widgets::SwitchWidget *m_smartMirrorBtn;
    dcc::widgets::NextPageWidget *m_updateMirrors;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
