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
    case ItemTitleRole:
        return optionName(index.row());
    case ItemDescriptionRole:
        return optionDescription(index.row());
    case ItemTypeRole:
        return QVariant::fromValue(optionType(index.row()));
    case ItemNameRole:
        return m_displayModel->monitorList()[index.row() - 2]->name();
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
        return tr("Only Displayed on %1").arg(m_displayModel->monitorList()[index - 2]->name());

    return tr("My Settings");
}

const QString DisplayControlModel::optionDescription(const int index) const
{
    if (index == 0)
        return tr("Copy the screen contents to one or more screens");
    else if (index == 1)
        return tr("Extend the screen contents to display different contents on different screens");
    else if (index < m_displayModel->monitorList().size() + 2)
        return tr("Screen contents are only displayed on %1").arg(m_displayModel->monitorList()[index - 2]->name());

    return tr("Please enter display mode to set if you want to change the custom settings");
}

DisplayControlModel::ItemType DisplayControlModel::optionType(const int index) const
{
    if (index == 0)
        return Duplicate;
    else if (index == 1)
        return Extend;
    else if (index < m_displayModel->monitorList().size() + 2)
        return Specificed;

    return Custom;
}
