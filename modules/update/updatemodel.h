#ifndef UPDATEMODEL_H
#define UPDATEMODEL_H

#include <QObject>
#include <com_deepin_lastore_updater.h>

namespace dcc{
namespace update{

class UpdateModel : public QObject
{
    Q_OBJECT
public:
    explicit UpdateModel(QObject *parent = 0);
    QString mirror() const { return m_mirror; }
    void setMirrorInfoList(const MirrorInfoList& list);
    MirrorInfoList mirrorInfos() const { return m_mirrorList;}

signals:
    void autoUpdate(bool update);
    void appInfos(const QList<AppUpdateInfo>& infos);
    void packageDownloadSize(qlonglong size, int count);
    void progressChanged(double progress);
    void statusChanged(bool useBattery, double percent);

public slots:
    void setDefaultMirror(const QString& mirror);
    void setAutoUpdate(bool update);

private:
    bool m_autoUpdate;
    QString m_mirror;
    MirrorInfoList m_mirrorList;
};

}
}
#endif // UPDATEMODEL_H
