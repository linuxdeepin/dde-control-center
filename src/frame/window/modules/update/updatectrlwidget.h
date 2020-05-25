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

#include "interface/namespace.h"
#include "widgets/contentwidget.h"
#include "modules/update/common.h"

#include <QWidget>

class AppUpdateInfo;
class QPushButton;

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace dcc {
namespace update {
class UpdateModel;
class DownloadInfo;
class SummaryItem;
class DownloadProgressBar;
class ResultItem;
}

namespace widgets {
class SettingsGroup;
class TipsLabel;
}
}

namespace DCC_NAMESPACE {
namespace update {

class LoadingItem;

class UpdateCtrlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateCtrlWidget(dcc::update::UpdateModel *model, QWidget *parent = 0);
    ~UpdateCtrlWidget();

    void setModel(dcc::update::UpdateModel *model);
    void setSystemVersion(QString version);

Q_SIGNALS:
    void requestDownloadUpdates();
    void requestPauseDownload();
    void requestResumeDownload();
    void requestInstallUpdates();
    void notifyUpdateState(int);

private Q_SLOTS:
    void onProgressBarClicked();
    void loadAppList(const QList<AppUpdateInfo> &infos);

private:
    void setStatus(const dcc::update::UpdatesStatus &status);
    void setDownloadInfo(dcc::update::DownloadInfo *downloadInfo);
    void setProgressValue(const double value);
    void setLowBattery(const bool &lowBattery);
    void setUpdateProgress(const double value);
    void setRecoverBackingUp(const bool value);
    void setRecoverConfigValid(const bool value);
    void setRecoverRestoring(const bool value);
    void setShowInfo(const bool value);

private:
    dcc::update::UpdateModel *m_model;
    dcc::update::UpdatesStatus m_status;
    LoadingItem *m_checkUpdateItem;
    dcc::update::ResultItem *m_resultItem;
    dcc::update::DownloadProgressBar *m_progress;
    dcc::update::DownloadProgressBar *m_fullProcess;
    dcc::widgets::SettingsGroup *m_summaryGroup;
    dcc::widgets::SettingsGroup *m_upgradeWarningGroup;
    dcc::update::SummaryItem *m_summary;
    dcc::update::SummaryItem *m_upgradeWarning;
    dcc::widgets::TipsLabel *m_powerTip;
    dcc::widgets::TipsLabel *m_reminderTip;
    dcc::widgets::TipsLabel *m_noNetworkTip;
    QSettings *m_qsettings;
    QString m_systemVersion;
    bool m_bRecoverBackingUp;
    bool m_bRecoverConfigValid;
    bool m_bRecoverRestoring;
    dcc::ContentWidget *m_updateList;
    dcc::widgets::TipsLabel *m_authorizationPrompt;

    QPushButton *m_checkUpdateBtn;
    dcc::widgets::TipsLabel *m_lastCheckTimeTip;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
