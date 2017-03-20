#include "miracastcontrolmodel.h"

MiracastControlModel::MiracastControlModel(MiracastModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_miracastModel(model)
{
    connect(m_miracastModel, &MiracastModel::linkAdded, this, &MiracastControlModel::onLinkAdded);
    connect(m_miracastModel, &MiracastModel::peerAdded, this, &MiracastControlModel::onPeerAdded);

    qRegisterMetaType<ItemInfo>("ItemInfo");
}

int MiracastControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    int count = 0;
    for (const auto &link : m_miracastModel->links())
        count += m_datas[link.m_dbusPath.path()].size() + 1;

    return count;
}

QVariant MiracastControlModel::data(const QModelIndex &index, int role) const
{
    const ItemInfo info = itemInfo(index.row());

    switch (role)
    {
    case MiracastDisplayRole:
        if (info.m_peer)
            return info.m_peer->m_name;
        else
            return info.m_link->m_name;
    case MiracastItemInfoRole:
        return QVariant::fromValue(info);
    }

    return QVariant();
}

void MiracastControlModel::onLinkAdded(const LinkInfo &link)
{
    qDebug() << Q_FUNC_INFO << link;

    m_datas.insert(link.m_dbusPath.path(), QList<PeerInfo>());

    emit layoutChanged();
}

void MiracastControlModel::onPeerAdded(const PeerInfo &peer)
{
    qDebug() << Q_FUNC_INFO << peer;

    const QString link = peer.m_linkPath.path();
    Q_ASSERT(m_datas.contains(link));
    Q_ASSERT(!m_datas[link].contains(peer));

    m_datas[link].append(peer);

    emit layoutChanged();
}

ItemInfo MiracastControlModel::itemInfo(const int row) const
{
    ItemInfo info = { nullptr, nullptr };

    int r = row;
    for (auto &link : m_miracastModel->links())
    {
        const QString linkPath = link.m_dbusPath.path();

        info.m_link = &link;

        if (!r)
            return info;
        else
            --r;

        const int s = m_datas[linkPath].size();
        if (r < s)
        {
            info.m_peer = &m_datas[linkPath][r];
            return info;
        } else {
            r -= s;
        }
    }

    info.m_link = nullptr;

    return info;
}
