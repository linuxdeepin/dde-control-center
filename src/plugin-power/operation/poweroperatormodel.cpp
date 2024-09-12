//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "poweroperatormodel.h"

PowerOperatorModel::PowerOperatorModel(QObject *parent)
: QAbstractListModel(parent)
{
    appendRow(new PowerOperator(0, tr("Shut down"), true, true));
    appendRow(new PowerOperator(1, tr("Suspend"), true, true));
    appendRow(new PowerOperator(2, tr("Hibernate"), true, true));
    appendRow(new PowerOperator(3, tr("Turn off the monitor"), true, true));
    appendRow(new PowerOperator(4, tr("Show the shutdown Interface"), true, true));
    appendRow(new PowerOperator(5, tr("Do nothing"), true, true));
}

PowerOperatorModel::~PowerOperatorModel()
{
    qDeleteAll(m_powerOperatorList);
    m_powerOperatorList.resize(0);
}

int PowerOperatorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_powerOperatorList.size();
}

QVariant PowerOperatorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) 
        return {};
    if (index.row() < 0 || index.row() >= m_powerOperatorList.size())
        return {};
    const auto operatorItem = m_powerOperatorList[index.row()];
    switch (role) {
    case KeyRole:
        return operatorItem->key;
    case TextRole:
        return operatorItem->text;
    case EnableRole:
        return operatorItem->enable;
    case VisibleRole:
        return operatorItem->visible;
    }

    return QVariant();
}

void PowerOperatorModel::appendRow(PowerOperator *powerOperator)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_powerOperatorList.append(powerOperator);
    endInsertRows();
}

void PowerOperatorModel::setEnable(int index, bool enable)
{
    if (index >= 0 && index < m_powerOperatorList.size()) {
        m_powerOperatorList[index]->enable = enable;
        emit dataChanged(PowerOperatorModel::index(index, 0), PowerOperatorModel::index(index, 0));
    }
}

void PowerOperatorModel::setVisible(int index, bool visible)
{
    if (index >= 0 && index < m_powerOperatorList.size()) {
        m_powerOperatorList[index]->visible = visible;
        emit dataChanged(PowerOperatorModel::index(index, 0), PowerOperatorModel::index(index, 0));
    }
}

QHash<int, QByteArray> PowerOperatorModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[KeyRole] = "key";
    roles[TextRole] = "text";
    roles[EnableRole] = "enable";
    roles[VisibleRole] = "visible";
    return roles;
}

quint8 PowerOperatorModel::keyOfIndex(int rowIndex) const
{
    if (rowIndex < 0 || rowIndex >= m_powerOperatorList.size()) {
        return -1;
    }
    return m_powerOperatorList[rowIndex]->key;
}

int PowerOperatorModel::indexOfKey(quint8 key) const
{
    for (int i = 0; i < m_powerOperatorList.size(); i++) {
        if (m_powerOperatorList[i]->key == key) {
            return i;
        }
    }
    return -1;
}