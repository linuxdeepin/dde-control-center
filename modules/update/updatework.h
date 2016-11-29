#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include <QObject>
#include <com_deepin_lastore_updater.h>
#include <com_deepin_lastore_job.h>
#include <com_deepin_lastore_jobmanager.h>

using UpdateInter=com::deepin::lastore::Updater;
using JobInter=com::deepin::lastore::Job;
using ManagerInter=com::deepin::lastore::Manager;

class UpdateWork : public QObject
{
    Q_OBJECT
public:
    explicit UpdateWork(QObject *parent = 0);
    MirrorInfoList mirrorInfos() const;
    QString defaultMirror() const;
    void setMirrorSource(const QString& src);

    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);

signals:
    void mirrorSourceChanged(const QString& src);
    bool autoCheckUpdatesChanged(bool update);

public slots:

private:
    UpdateInter* m_updateInter;
    JobInter* m_jobInter;
    ManagerInter* m_managerInter;
};

#endif // UPDATEWORK_H
