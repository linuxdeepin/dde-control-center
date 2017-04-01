#ifndef MIRACASTCONTROLMODEL_H
#define MIRACASTCONTROLMODEL_H

#include "miracastmodel.h"

#include <QAbstractListModel>

struct ItemInfo
{
    const LinkInfo *m_link;
    const SinkInfo *m_sink;
};

class MiracastControlModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum MiracastItemRole
    {
        MiracastItemSizeHintRole = Qt::SizeHintRole,
        MiracastDisplayRole = Qt::DisplayRole,
        MiracastReservedRole = Qt::UserRole,
        MiracastActiveRole,
        MiracastItemInfoRole,
    };

    explicit MiracastControlModel(MiracastModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private slots:
    void onLinkAdded(const LinkInfo &link);
    void onLinkRemoved(const QDBusObjectPath &path);
    void onSinkAdded(const SinkInfo &sink);
    void onSinkRemoved(const SinkInfo &sink);
    void onSinkConnectedChanged(const QDBusObjectPath &path, const bool connected);

private:
    ItemInfo itemInfo(const int row) const;

private:
    MiracastModel *m_miracastModel;

    QMap<QString, QList<SinkInfo>> m_datas;
};

Q_DECLARE_METATYPE(ItemInfo)

#endif // MIRACASTCONTROLMODEL_H
