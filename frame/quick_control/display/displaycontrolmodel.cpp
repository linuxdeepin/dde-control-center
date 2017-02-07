#include "displaycontrolmodel.h"

#include <QSize>

using dcc::display::DisplayModel;

DisplayControlModel::DisplayControlModel(DisplayModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_displayModel(model)
{
    connect(m_displayModel, &DisplayModel::monitorListChanged, [=] { emit layoutChanged(); });
}

int DisplayControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int configCount = 2 + m_displayModel->monitorList().size();

    return configCount + m_displayModel->hasConfig();
}

QVariant DisplayControlModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case TypeNameRole:
        return optionName(index.row());
    case Qt::SizeHintRole:
        return QSize(0, 50);
    default:;
    }

    return QVariant();
}

const QString DisplayControlModel::optionName(const int index) const
{
    if (index == 0)
        return tr("Copy");
    else if (index == 1)
        return tr("Extend");
    else if (index < m_displayModel->monitorList().size() + 2)
        return tr("Screen contents are only displayed on %1").arg(m_displayModel->monitorList()[index - 2]->name());

    return tr("My Settings");
}
