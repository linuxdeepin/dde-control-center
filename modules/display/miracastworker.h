#ifndef MIRACASTWORKER_H
#define MIRACASTWORKER_H

#include <QObject>
#include <QTimer>
#include <com_deepin_daemon_miracast.h>

using MiracastInter = com::deepin::daemon::Miracast;

namespace dcc {
namespace display {
class MiracastModel;
class MiracastWorker : public QObject
{
    Q_OBJECT

public:
    explicit MiracastWorker(MiracastModel *model, QObject *parent = 0);
    void active();
    void deactive();
    void fetchData();
    void queryLinks();
    void querySinks();

public slots:
    void disconnectSink(const QDBusObjectPath &sink);
    void connectSink(const QDBusObjectPath &peer, const QRect area);
    void setLinkEnable(const QDBusObjectPath &path, const bool enable);
    void setLinkScannning(const QDBusObjectPath &path, const bool scanning);

private slots:
    void queryLinks_CB(QDBusPendingCallWatcher *w);
    void querySinks_CB(QDBusPendingCallWatcher *w);
    void onPowerOff();

private:
    MiracastModel *m_miracastModel;
    MiracastInter *m_miracastInter;
    QTimer *m_timerPowerOff;
};
}
}

#endif // MIRACASTWORKER_H
