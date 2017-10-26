#include "fingerworker.h"

using namespace dcc;
using namespace dcc::accounts;

const QString FprintService("com.deepin.daemon.Fprintd");

FingerWorker::FingerWorker(FingerModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_fprintdInter(new Fprintd(FprintService, "/com/deepin/daemon/Fprintd", QDBusConnection::sessionBus(), this))
    , m_fprDefaultInter(nullptr)
{
    m_fprintdInter->setSync(false);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_fprintdInter->GetDefaultDevice(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &FingerWorker::onGetFprDefaultDevFinished);
}

void FingerWorker::refreshUserEnrollList(const QString &name)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_fprDefaultInter->ListEnrolledFingers(name), this);
    watcher->setProperty("user", name);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &FingerWorker::onGetListEnrolledFinished);
}

void FingerWorker::enrollStart(const QString &name, const QString &thumb)
{
    // clean
    m_fprDefaultInter->EnrollStop();

    QTimer::singleShot(100, this, [=] { m_fprDefaultInter->Release();});

    QTimer::singleShot(200, this, [=] {
        m_fprDefaultInter->Claim(name);
    });
    QTimer::singleShot(300, this, [=] {
        m_fprDefaultInter->EnrollStart(thumb);
        m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
    });
}

void FingerWorker::reEnrollStart(const QString &thumb)
{
    m_fprDefaultInter->EnrollStop();

    QTimer::singleShot(100, this, [=] {
        m_fprDefaultInter->EnrollStart(thumb);
        m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
    });
}

void FingerWorker::cleanEnroll(User *user)
{
    m_fprDefaultInter->DeleteEnrolledFingers(user->name());

    m_model->cleanUserThumbs(user->name());
    m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
}

void FingerWorker::saveEnroll(const QString &name)
{
    m_fprDefaultInter->EnrollStop();

    QTimer::singleShot(100, this, [=] {
        refreshUserEnrollList(name);
    });
    m_model->setEnrollStatus(FingerModel::EnrollStatus::Ready);
}

void FingerWorker::onGetFprDefaultDevFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QDBusObjectPath> reply = w->reply();

    w->deleteLater();

    const QDBusObjectPath &path = reply.value();

    if (path.path().isEmpty())
        return;

    m_fprDefaultInter = new Device(FprintService, path.path(), QDBusConnection::sessionBus(), this);

    m_model->setIsVaild(m_fprDefaultInter->isValid());

    connect(m_fprDefaultInter, &Device::EnrollStatus, this, &FingerWorker::onEnrollStatus);
}

void FingerWorker::onGetListEnrolledFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QStringList> reply = w->reply();

    if (!reply.isError()) {
        FingerModel::UserThumbs u;
        u.username = w->property("user").toString();
        u.userThumbs = reply.value();
        m_model->addUserThumbs(u);
    } else {
        qDebug() << reply.error();
    }

    w->deleteLater();
}

void FingerWorker::onEnrollStatus(const QString &value, const bool status)
{
    if (value == "enroll-completed" && status == 1) {
        m_model->setEnrollStatus(FingerModel::EnrollStatus::Finished);
        return;
    }

    m_model->setEnrollStatus(FingerModel::EnrollStatus::Next);
}
