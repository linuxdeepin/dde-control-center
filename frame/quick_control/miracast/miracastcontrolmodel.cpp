#include "miracastcontrolmodel.h"

MiracastControlModel::MiracastControlModel(MiracastModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_miracastModel(model)
{
    connect(m_miracastModel, &MiracastModel::linkAdded, this, &MiracastControlModel::onLinkAdded);
    connect(m_miracastModel, &MiracastModel::linkRemoved, this, &MiracastControlModel::onLinkRemoved);
    connect(m_miracastModel, &MiracastModel::peerAdded, this, &MiracastControlModel::onSinkAdded);
    connect(m_miracastModel, &MiracastModel::peerRemoved, this, &MiracastControlModel::onSinkRemoved);

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
        if (info.m_sink)
            return info.m_sink->m_name + " " + (info.m_sink->m_connected ? "true" : "false");
        else
            return info.m_link->m_name + " " + (info.m_link->m_managed ? "true" : "false");
    case MiracastItemInfoRole:
        return QVariant::fromValue(info);
    }

    return QVariant();
}

void MiracastControlModel::onLinkAdded(const LinkInfo &link)
{
    qDebug() << Q_FUNC_INFO << link;

    m_datas.insert(link.m_dbusPath.path(), QList<SinkInfo>());

    emit layoutChanged();
}

void MiracastControlModel::onLinkRemoved(const QDBusObjectPath &path)
{
    qDebug() << Q_FUNC_INFO << path.path();

    m_datas.remove(path.path());

    emit layoutChanged();
}

void MiracastControlModel::onSinkAdded(const SinkInfo &sink)
{
    qDebug() << Q_FUNC_INFO << sink;

    const QString link = sink.m_linkPath.path();
    Q_ASSERT(m_datas.contains(link));
    Q_ASSERT(!m_datas[link].contains(sink));

    m_datas[link].append(sink);

    emit layoutChanged();
}

void MiracastControlModel::onSinkRemoved(const SinkInfo &sink)
{
    const QString link = sink.m_linkPath.path();
    Q_ASSERT(m_datas.contains(link));

    m_datas[link].removeAll(sink);

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
            info.m_sink = &m_datas[linkPath][r];
            return info;
        } else {
            r -= s;
        }
    }

    info.m_link = nullptr;

    return info;
}
