#ifndef UPDATECTRLWIDGET_H
#define UPDATECTRLWIDGET_H

#include "contentwidget.h"
#include "settingsgroup.h"
#include "summaryitem.h"
#include "downloadprogressbar.h"

#include <types/appupdateinfolist.h>

#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>
#include <com_deepin_lastore_jobmanager.h>
#include <com_deepin_daemon_power.h>

using UpdateInter=com::deepin::lastore::Updater;
using JobInter=com::deepin::lastore::Job;
using ManagerInter=com::deepin::lastore::Manager;
using PowerInter=com::deepin::daemon::Power;

using namespace dcc;
using namespace dcc::widgets;

namespace dcc{
namespace update{

class UpdateCtrlWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit UpdateCtrlWidget(QWidget *parent = 0);
    ~UpdateCtrlWidget();

    static inline QString formatCap(qulonglong cap, const int size = 1024)
    {
        static QString type[] = {"B", "KB", "MB", "GB", "TB"};

        if (cap < qulonglong(size)) {
            return QString::number(cap) + type[0];
        }
        if (cap < qulonglong(size) * size) {
            return QString::number(double(cap) / size, 'f', 2) + type[1];
        }
        if (cap < qulonglong(size) * size * size) {
            return QString::number(double(cap) / size / size, 'f', 2) + type[2];
        }
        if (cap < qulonglong(size) * size * size * size) {
            return QString::number(double(cap) / size / size / size, 'f', 2) + type[3];
        }

        return QString::number(double(cap) / size / size / size / size, 'f', 2) + type[4];
    }

    void setCurState(UpdateType type);

signals:
    void actionType(UpdateType type);

public slots:
    void loadAppList(const QList<AppUpdateInfo> &infos);
    void updateDownloadProgress(double progress);
    void onStatus(bool useBattery, double percent);
    void onPackagesDownloadSize(qlonglong size, int count);

private:
    QLabel* m_powerTip;
    qlonglong m_total;
    SummaryItem* m_summary;
    CheckUpdateItem* m_checkUpdateItem;
    SettingsGroup* m_group;
    SettingsGroup* m_checkGroup;
    DownloadProgressBar* m_progress;
};

}
}
#endif // UPDATECTRLWIDGET_H
