#include "displaycontrolmodel.h"
#include "display/displayworker.h"

#include <QSize>
#include <QPixmap>

using dcc::display::DisplayModel;

DisplayControlModel::DisplayControlModel(DisplayModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_displayModel(model)
{
    connect(m_displayModel, &DisplayModel::displayModeChanged, this, &DisplayControlModel::onDisplayModeChanged, Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::primaryScreenChanged, this, [this] { onDisplayModeChanged(m_displayModel->displayMode()); }, Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::monitorListChanged, [=] { emit layoutChanged(); });

    onDisplayModeChanged(m_displayModel->displayMode());
}

int DisplayControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    const int configCount = 2 + m_displayModel->monitorList().size();

    return configCount + m_displayModel->configList().size();
}

QVariant DisplayControlModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case ItemSelectedRole:
        return m_selectedIndex == index;
    case ItemTitleRole:
        return optionName(index.row());
    case ItemDescriptionRole:
        return optionDescription(index.row());
    case ItemTypeRole:
        return QVariant::fromValue(optionType(index.row()));
    case ItemNameRole:
        return m_displayModel->monitorList()[index.row() - 2]->name();
    case ItemIsLastRole:
        return index.row() == 2 + m_displayModel->monitorList().size() + m_displayModel->configList().size() - 1;
    case ItemIconRole:
        return optionIcon(index.row());
    case Qt::SizeHintRole:
        return QSize(0, 70);
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

void DisplayControlModel::onDisplayModeChanged(const int mode)
{
    const QModelIndex oldIndex = m_selectedIndex;

    if (mode == MERGE_MODE)
        m_selectedIndex = index(0);
    else if (mode == EXTEND_MODE)
        m_selectedIndex = index(1);
    else if (mode == CUSTOM_MODE)
        m_selectedIndex = index(rowCount(QModelIndex()) - 1); // TODO: support multi config file
    else
    {
        int idx = 2;
        const QString current = m_displayModel->primary();
        for (const auto *mon : m_displayModel->monitorList())
        {
            if (mon->name() == current)
                break;

            ++idx;
        }

        m_selectedIndex = index(idx);
    }

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(m_selectedIndex, m_selectedIndex);
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

const QPixmap DisplayControlModel::optionIcon(const int index) const
{
    const ItemType type = optionType(index);

    switch (type)
    {
    case Duplicate:     return QPixmap(":/frame/themes/dark/icons/copy_mode.png");
    case Extend:        return QPixmap(":/frame/themes/dark/icons/extend_mode.png");
    case Custom:        return QPixmap(":/frame/themes/dark/icons/custom.png");
    case Specificed:    return QPixmap(index > 2 ? ":/frame/themes/dark/icons/only2.png" : ":/frame/themes/dark/icons/only1.png");
    default:;
    }

    Q_UNREACHABLE();

    return QPixmap();
}
