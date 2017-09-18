/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef UPDATECTRLWIDGET_H
#define UPDATECTRLWIDGET_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "summaryitem.h"
#include "downloadprogressbar.h"
#include "resultitem.h"

#include <types/appupdateinfolist.h>

#include "labels/tipslabel.h"

using namespace dcc;
using namespace dcc::widgets;

namespace dcc{
namespace update{

class UpdateModel;
class DownloadInfo;
class LoadingItem;

class UpdateCtrlWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateCtrlWidget(UpdateModel *model, QWidget *parent = 0);
    ~UpdateCtrlWidget();

    void setModel(UpdateModel *model);

signals:
    void requestDownloadUpdates();
    void requestPauseDownload();
    void requestResumeDownload();
    void requestInstallUpdates();
    void suggestReboot();

private slots:
    void onProgressBarClicked();
    void loadAppList(const QList<AppUpdateInfo> &infos);

private:
    void setStatus(const UpdatesStatus &status);
    void setDownloadInfo(DownloadInfo *downloadInfo);

    void setLowBattery(const bool &lowBattery);

private:
    UpdateModel *m_model;
    UpdatesStatus m_status;

    SettingsGroup* m_checkGroup;
    LoadingItem* m_checkUpdateItem;
    SettingsGroup *m_resultGroup;
    ResultItem *m_resultItem;
    DownloadProgressBar* m_progress;
    SettingsGroup* m_summaryGroup;
    SummaryItem* m_summary;
    TipsLabel *m_powerTip;
    TipsLabel *m_reminderTip;
};

}
}
#endif // UPDATECTRLWIDGET_H
