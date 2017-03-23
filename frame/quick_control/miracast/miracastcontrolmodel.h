#ifndef MIRACASTCONTROLMODEL_H
#define MIRACASTCONTROLMODEL_H

#include "miracastmodel.h"

#include <QAbstractListModel>

struct ItemInfo
{
    const LinkInfo *m_link;
    const PeerInfo *m_peer;
};

class MiracastControlModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum MiracastItemRole
    {
        MiracastDisplayRole = Qt::DisplayRole,
        MiracastReservedRole = Qt::UserRole,
        MiracastItemInfoRole,
    };

    explicit MiracastControlModel(MiracastModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private slots:
    void onLinkAdded(const LinkInfo &link);
    void onLinkRemoved(const QDBusObjectPath &path);
    void onPeerAdded(const PeerInfo &peer);

private:
    ItemInfo itemInfo(const int row) const;

private:
    MiracastModel *m_miracastModel;

    QMap<QString, QList<PeerInfo>> m_datas;
};

Q_DECLARE_METATYPE(ItemInfo)

#endif // MIRACASTCONTROLMODEL_H
