#include "miracastworker.h"
#include "miracastmodel.h"

MiracastWorker::MiracastWorker(MiracastModel *model, QObject *parent)
    : QObject(parent),

      m_miracastModel(model),
      m_miracastInter(new MiracastInter("com.deepin.daemon.Miracast", "/com/deepin/daemon/Miracast", QDBusConnection::sessionBus(), this))
{
    m_miracastInter->setSync(false);

    connect(m_miracastInter, &MiracastInter::Added, m_miracastModel, &MiracastModel::onPathAdded);
    connect(m_miracastInter, &MiracastInter::Removed, m_miracastModel, &MiracastModel::onPathRemoved);
    connect(m_miracastInter, &MiracastInter::Event, m_miracastModel, &MiracastModel::onMiracastEvent);

    QTimer::singleShot(1, this, &MiracastWorker::fetchData);
}

void MiracastWorker::fetchData()
{
    queryLinks();
}

void MiracastWorker::queryLinks()
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_miracastInter->ListLinks());

    connect(w, &QDBusPendingCallWatcher::finished, this, &MiracastWorker::queryLinks_CB);
}

void MiracastWorker::connectPeer(const QDBusObjectPath &peer, const QRect area)
{
    qDebug() << Q_FUNC_INFO << peer.path() << area;

    m_miracastInter->Connect(peer, area.x(), area.y(), area.width(), area.height());
}

void MiracastWorker::setLinkEnable(const QDBusObjectPath &path, const bool enable)
{
    qDebug() << Q_FUNC_INFO << path.path() << enable;

    m_miracastInter->Enable(path, enable);
}

void MiracastWorker::setLinkScannning(const QDBusObjectPath &path, const bool scanning)
{
    qDebug() << Q_FUNC_INFO << path.path() << scanning;

    m_miracastInter->Scanning(path, scanning);
}

void MiracastWorker::queryLinks_CB(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<LinkInfoList> reply = *w;

    m_miracastModel->setLinks(reply.value());

    w->deleteLater();
}
