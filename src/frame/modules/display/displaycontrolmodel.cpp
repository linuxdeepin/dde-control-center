/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "displaycontrolmodel.h"
#include "displayworker.h"

#include <QSize>
#include <QPixmap>

using dcc::display::DisplayModel;

DisplayControlModel::DisplayControlModel(DisplayModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_displayModel(model)
{
    connect(m_displayModel, &DisplayModel::displayModeChanged, this, &DisplayControlModel::onDisplayModeChanged, Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::monitorListChanged, this, [=] { Q_EMIT layoutChanged(); }, Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::configListChanged, this, [=] { Q_EMIT layoutChanged(); }, Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::primaryScreenChanged, this, [=] { onDisplayModeChanged(m_displayModel->displayMode()); }, Qt::QueuedConnection);
    connect(m_displayModel, &DisplayModel::currentConfigChanged, this, [=] { onDisplayModeChanged(m_displayModel->displayMode()); }, Qt::QueuedConnection);
    connect(this, &DisplayControlModel::layoutChanged, this, [=] { onDisplayModeChanged(m_displayModel->displayMode()); }, Qt::QueuedConnection);

    onDisplayModeChanged(m_displayModel->displayMode());
}

int DisplayControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_displayModel->monitorList().size() + 2;
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
        return index.row() == rowCount(QModelIndex()) - 1;
    case ItemIconRole:
        return optionIcon(index.row());
    case ItemConfigNameRole:
        return m_displayModel->configList()[index.row() - 2 - m_displayModel->monitorList().size()];
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
    else
        return QString();
}

const QString DisplayControlModel::optionDescription(const int index) const
{
    if (index == 0)
        return tr("Copy the screen contents to one or more screens");
    else if (index == 1)
        return tr("Extend the screen contents to display different contents on different screens");
    else if (index < m_displayModel->monitorList().size() + 2)
        return tr("Screen contents are only displayed on %1").arg(m_displayModel->monitorList()[index - 2]->name());
    else
        return QString();
}

void DisplayControlModel::onDisplayModeChanged(const int mode)
{
    const QModelIndex oldIndex = m_selectedIndex;

    if (mode == MERGE_MODE)
        m_selectedIndex = index(0);
    else if (mode == EXTEND_MODE)
        m_selectedIndex = index(1);
    else if (mode == CUSTOM_MODE)
        m_selectedIndex = index(2 + m_displayModel->monitorList().size() + m_displayModel->configList().indexOf(m_displayModel->config()));
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

    Q_EMIT dataChanged(oldIndex, oldIndex);
    Q_EMIT dataChanged(m_selectedIndex, m_selectedIndex);
}

DisplayControlModel::ItemType DisplayControlModel::optionType(const int index) const
{
    if (index == 0)
        return Duplicate;
    else if (index == 1)
        return Extend;
    else if (index < m_displayModel->monitorList().size() + 2)
        return Specified;
    else if (index != rowCount(QModelIndex()) - 1)
        return Custom;

    return NewConfig;
}

const QPixmap DisplayControlModel::optionIcon(const int index) const
{
    const ItemType type = optionType(index);

    switch (type)
    {
    case Duplicate:     return QPixmap(":/frame/themes/dark/icons/copy_mode.png");
    case Extend:        return QPixmap(":/frame/themes/dark/icons/extend_mode.png");
    case NewConfig:
    case Custom:        return QPixmap(":/frame/themes/dark/icons/custom.png");
    case Specified:     return QPixmap(index > 2 ? ":/frame/themes/dark/icons/only2.png" : ":/frame/themes/dark/icons/only1.png");
    default:;
    }

    Q_UNREACHABLE();

    return QPixmap();
}
