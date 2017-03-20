#ifndef MIRACASTWORKER_H
#define MIRACASTWORKER_H

#include <QObject>

#include <com_deepin_daemon_miracast.h>

using MiracastInter = com::deepin::daemon::Miracast;

class MiracastModel;
class MiracastWorker : public QObject
{
    Q_OBJECT

public:
    explicit MiracastWorker(MiracastModel *model, QObject *parent = 0);

    void fetchData();
    void queryLinks();

public slots:
    void connectPeer(const QDBusObjectPath &peer, const QRect area);
    void setLinkEnable(const QDBusObjectPath &path, const bool enable);
    void setLinkScannning(const QDBusObjectPath &path, const bool scanning);

private slots:
    void queryLinks_CB(QDBusPendingCallWatcher *w);

private:
    MiracastModel *m_miracastModel;
    MiracastInter *m_miracastInter;
};

#endif // MIRACASTWORKER_H
