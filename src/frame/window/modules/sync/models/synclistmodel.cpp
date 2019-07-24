#include "synclistmodel.h"

#include "modules/sync/syncmodel.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncListModel::SyncListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void SyncListModel::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;
}

int SyncListModel::rowCount(const QModelIndex &parent) const
{
    return m_model->moduleMap().size();
}

QVariant SyncListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::WhatsThisRole:
        break;
    case Qt::DisplayRole:
        return translation(index.row());
    case Qt::DecorationRole:
        return QVariant();
    default:;
    }

    return QVariant();
}

const QString SyncListModel::translation(int index) const
{
    QStringList moduleTs {
        tr("Network Settings"),
        tr("Sound Settings"),
        tr("Mouse Settings"),
        tr("Update Settings"),
        tr("Dock"),
        tr("Launcher"),
        tr("Wallpaper"),
        tr("Theme"),
        tr("Power Settings"),
        tr("Corner Settings"),
    };

    return moduleTs.value(index);
}
