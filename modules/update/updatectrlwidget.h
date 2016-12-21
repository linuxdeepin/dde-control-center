#ifndef UPDATECTRLWIDGET_H
#define UPDATECTRLWIDGET_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "summaryitem.h"
#include "downloadprogressbar.h"

#include <types/appupdateinfolist.h>

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
    DownloadInfo *m_downloadInfo;

    SettingsGroup* m_checkGroup;
    LoadingItem* m_checkUpdateItem;
    DownloadProgressBar* m_progress;
    SettingsGroup* m_summaryGroup;
    SummaryItem* m_summary;
    NormalLabel* m_powerTip;
};

}
}
#endif // UPDATECTRLWIDGET_H
