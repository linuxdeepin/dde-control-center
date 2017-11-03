#ifndef FINGERWORKER_H
#define FINGERWORKER_H

#include "fingermodel.h"
#include "user.h"

#include <com_deepin_daemon_fprintd.h>
#include <com_deepin_daemon_fprintd_device.h>

#include <QObject>

using com::deepin::daemon::Fprintd;
using com::deepin::daemon::fprintd::Device;

namespace dcc {
namespace accounts {

class FingerWorker : public QObject
{
    Q_OBJECT
public:
    explicit FingerWorker(FingerModel *model, QObject *parent = nullptr);

signals:
    void requestShowAddThumb(const QString &name, const QString &thumb);

public slots:
    void refreshUserEnrollList(const QString &name);
    void enrollStart(const QString &name, const QString &thumb);
    void reEnrollStart(const QString &thumb);
    void cleanEnroll(User *user);
    void saveEnroll(const QString &name);
    void stopEnroll();

private slots:
    void onGetFprDefaultDevFinished(QDBusPendingCallWatcher *w);
    void onGetListEnrolledFinished(QDBusPendingCallWatcher *w);
    void onEnrollStatus(const QString &value, const bool status);
    bool recordFinger(const QString &name, const QString &thumb);
    bool reRecordFinger(const QString &thumb);
    bool saveFinger();

private:
    FingerModel *m_model;
    Fprintd *m_fprintdInter;
    Device* m_fprDefaultInter;
};

}
}

#endif // FINGERWORKER_H
