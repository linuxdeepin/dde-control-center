// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"
#include "widgets/contentwidget.h"
#include "window/dconfigwatcher.h"

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

QT_BEGIN_NAMESPACE
class QGSettings;
QT_END_NAMESPACE

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
    void requestSetAutoInstall(const bool &autoInstall);
    void requestSetTestingChannelEnable(const bool &enable);
    void requestCheckCanExitTestingChannel();

private Q_SLOTS:
    void setUpdateMode();
    void setCheckStatus(QWidget *widget, bool state, const QString &key);
    void onAutoUpdateCheckChanged();
    void onAutoSecureUpdateCheckChanged();
    // When the testing channel switch chagned
    void onTestingChannelCheckChanged(const bool checked);
    // When the testing channel status changed
    void onTestingChannelStatusChanged();

private:
    void initUi();
    void initConnection();
    QString getAutoInstallUpdateType(quint64 type);
    void setAutoCheckEnable(bool enable);

private:
    dcc::update::UpdateModel *m_model;

    dcc::widgets::SwitchWidget *m_autoCheckUpdate;       // 检查更新
    dcc::widgets::SwitchWidget *m_autoCheckSecureUpdate; // 检查安全更新
    dcc::widgets::SwitchWidget *m_autoCheckUniontechUpdate; // 检查系统更新
    dcc::widgets::SwitchWidget *m_autoCheckAppUpdate;    // 检查应用商店的应用更新
    dcc::widgets::SwitchWidget *m_autoCheckThirdpartyUpdate; // 第三方仓库更新
    dcc::widgets::SwitchWidget *m_updateNotify;          // 更新提醒
    dcc::widgets::SwitchWidget *m_autoDownloadUpdate;    // 下载更新
    dcc::widgets::SwitchWidget *m_autoInstallUpdate;     // 安装更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoInstallUpdatesTips;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoDownloadUpdateTips;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_autoCheckSecureUpdateTips;

    dcc::widgets::SwitchWidget *m_testingChannel;        // Testing Channel Switch Button
    DTK_WIDGET_NAMESPACE::DTipLabel *m_testingChannelTips; // Testing Channle Description Label
    QLabel *m_testingChannelHeadingLabel; // Testing Channel Title
    QLabel *m_testingChannelLinkLabel; // Testing Channel Join Link

    dcc::widgets::SwitchWidget *m_timerDownload; // 定时下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_timerDownloadLbl;
    QLabel *m_setTimerLbl;

    dcc::widgets::SwitchWidget *m_freeTimeDownload; // 闲时下载更新
    DTK_WIDGET_NAMESPACE::DTipLabel *m_freeTimeDownloadLbl;
    QLabel *m_setFreeTimeLbl;

    dcc::widgets::SwitchWidget *m_autoCleanCache; // Clear Package Cache

#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    dcc::widgets::SwitchWidget *m_sourceCheck; // System Repository Detection
#endif
    dcc::widgets::SwitchWidget *m_smartMirrorBtn;
    dcc::widgets::NextPageWidget *m_updateMirrors;
    DConfig* m_dconfig;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
