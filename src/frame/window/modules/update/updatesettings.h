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

namespace DCC_NAMESPACE {
namespace update {

class UpdateSettings : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateSettings(dcc::update::UpdateModel *model, QWidget *parent = 0);

    void setModel(dcc::update::UpdateModel *model);

Q_SIGNALS:
    void requestShowMirrorsView();
    void requestSetAutoUpdate(const bool &autoUpdate);
    void requestSetAutoCleanCache(const bool autoClean);
    void requestSetAutoCheckUpdates(const bool autocheckUpdate);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    void requestSetSourceCheck(const bool check);
#endif
    void requestEnableSmartMirror(bool enable);
    void requestSetBootAutoCheck(bool check);
private:
    dcc::update::UpdateModel *m_model;
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    dcc::widgets::SwitchWidget *m_sourceCheck;
#endif
    dcc::widgets::SwitchWidget *m_bootCheckUpdate; //系统启动自动检测更新配置
    dcc::widgets::SwitchWidget *m_autoCleanCache; //自动清除缓存包配置
    dcc::widgets::SwitchWidget *m_autoCheckUpdate; //更新提醒配置
    dcc::widgets::SwitchWidget *m_autoDownloadSwitch; //自动下载更新配置
    dcc::widgets::SwitchWidget *m_smartMirrorBtn;
    dcc::widgets::NextPageWidget *m_updateMirrors;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_updateLbl;
    dcc::widgets::SettingsGroup *m_mirrorGrp;

    //定时下载更新
    dcc::widgets::SwitchWidget *m_timerDownload;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_timerDownloadLbl;
    QLabel *m_setTimerLbl;

    //闲时下载更新
    dcc::widgets::SwitchWidget *m_freeTimeDownload;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_freeTimeDownloadLbl;
    QLabel *m_setFreeTimeLbl;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
