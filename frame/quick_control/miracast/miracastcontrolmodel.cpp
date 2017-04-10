#include "miracastcontrolmodel.h"

MiracastControlModel::MiracastControlModel(MiracastModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_miracastModel(model)
{
    connect(m_miracastModel, &MiracastModel::linkAdded, this, &MiracastControlModel::onLinkAdded);
    connect(m_miracastModel, &MiracastModel::linkRemoved, this, &MiracastControlModel::onLinkRemoved);
    connect(m_miracastModel, &MiracastModel::peerAdded, this, &MiracastControlModel::onSinkAdded);
    connect(m_miracastModel, &MiracastModel::peerRemoved, this, &MiracastControlModel::onSinkRemoved);
    connect(m_miracastModel, &MiracastModel::sinkConnectedChanged, this, &MiracastControlModel::onSinkConnectedChanged);

    qRegisterMetaType<MiracastInfo>("MiracastInfo");
}

int MiracastControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    int count = 0;
    for (const auto &link : m_miracastModel->links())
        count += m_datas[link.m_dbusPath.path()].size() + 1;

    return count;
}

void MiracastControlModel::setCurrentHovered(const QModelIndex &index)
{
    m_currentIndex = index;

    emit dataChanged(m_currentIndex, m_currentIndex);
}

QVariant MiracastControlModel::data(const QModelIndex &index, int role) const
{
    const MiracastInfo info = itemInfo(index.row());

    switch (role)
    {
    case MiracastDisplayRole:
        if (info.m_sink)
            return info.m_sink->m_name;
        else
            return info.m_link->m_name;
    case MiracastActiveRole:
        if (info.m_sink)
            return info.m_sink->m_connected;
        else
            return info.m_link->m_managed;
    case MiracastItemInfoRole:
        return QVariant::fromValue(info);
    case MiracastItemSizeHintRole:
        if (info.m_sink && !info.m_link)
            return QSize(0, 40);
        else
            return QSize(0, 30);
    case MiracastItemConnectRole:
        return info.m_sink->m_connected;
    case MiracastItemHoverRole:
        return index.row() == m_currentIndex.row();
    case MiracastItemNextRole:
        return m_currentIndex.row() + 1 == index.row();
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

void MiracastControlModel::onSinkConnectedChanged(const QDBusObjectPath &path, const bool connected)
{
    qDebug() << Q_FUNC_INFO << path.path() << connected;

    for (auto i(m_datas.begin()); i != m_datas.end(); ++i)
    {
        for (auto &sink : i.value())
        {
            if (sink.m_sinkPath == path)
            {
                qDebug() << sink;

                sink.m_connected = connected;

                emit dataChanged(QModelIndex(), QModelIndex());
                return;
            }
        }
    }
}

MiracastInfo MiracastControlModel::itemInfo(const int row) const
{
    MiracastInfo info = { nullptr, nullptr };

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
