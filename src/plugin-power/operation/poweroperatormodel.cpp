//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "poweroperatormodel.h"

PowerOperatorModel::PowerOperatorModel(QObject *parent)
: QAbstractListModel(parent)
{
    appendRow(new PowerOperator(0, "关机", true, true));
    appendRow(new PowerOperator(1, "待机", true, true));
    appendRow(new PowerOperator(2, "休眠", true, true));
    appendRow(new PowerOperator(3, "关闭显示器", true, true));
    appendRow(new PowerOperator(4, "进入关机界面", true, true));
    appendRow(new PowerOperator(5, "无任何操作", true, true));
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
    case ValueRole:
        return operatorItem->value;
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
    if (index > 0 && index < m_powerOperatorList.size()) {
        m_powerOperatorList[index]->enable = enable;
        emit dataChanged(PowerOperatorModel::index(index, 0), PowerOperatorModel::index(index, 0));
    }
}

void PowerOperatorModel::setVisible(int index, bool visible)
{
    if (index > 0 && index < m_powerOperatorList.size()) {
        m_powerOperatorList[index]->visible = visible;
        emit dataChanged(PowerOperatorModel::index(index, 0), PowerOperatorModel::index(index, 0));
    }
}

QHash<int, QByteArray> PowerOperatorModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ValueRole] = "value";
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
    return m_powerOperatorList[rowIndex]->value;
}

int PowerOperatorModel::indexOfKey(quint8 key) const
{
    for (int i = 0; i < m_powerOperatorList.size(); i++) {
        if (m_powerOperatorList[i]->value == key) {
            return i;
        }
    }
    return -1;
}