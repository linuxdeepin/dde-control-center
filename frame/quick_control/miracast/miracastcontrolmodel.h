#ifndef MIRACASTCONTROLMODEL_H
#define MIRACASTCONTROLMODEL_H

#include "miracastmodel.h"

#include <QAbstractListModel>

struct MiracastInfo
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
        MiracastItemConnectRole,
        MiracastItemHoverRole,
        MiracastItemNextRole,
    };

    explicit MiracastControlModel(MiracastModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void setCurrentHovered(const QModelIndex &index);

private slots:
    void onLinkAdded(const LinkInfo &link);
    void onLinkRemoved(const QDBusObjectPath &path);
    void onSinkAdded(const SinkInfo &sink);
    void onSinkRemoved(const SinkInfo &sink);
    void onSinkConnectedChanged(const QDBusObjectPath &path, const bool connected);

private:
    MiracastInfo itemInfo(const int row) const;

private:
    MiracastModel *m_miracastModel;

    QMap<QString, QList<SinkInfo>> m_datas;
    QModelIndex m_currentIndex;
};

Q_DECLARE_METATYPE(MiracastInfo)

#endif // MIRACASTCONTROLMODEL_H
