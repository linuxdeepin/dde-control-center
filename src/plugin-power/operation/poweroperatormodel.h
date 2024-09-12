//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef POWEROPERATORMODEL_H
#define POWEROPERATORMODEL_H

#include <QtQml/qqml.h>
#include <QAbstractListModel>

enum PowerOperatorType{
    POT_ShutDown = 0,
    POT_Suspend,
    POT_Hibernate,
    POT_TurnOffMonitor,
    POT_ShowShutDownInter,
    POT_DoNoting
};

struct PowerOperator
{
    quint8 key;
    QString text;
    bool visible;
    bool enable;
    PowerOperator(quint8 key, QString text, bool visible = true, bool enable = true)
        : key(key), text(text), visible(visible), enable(enable) {}
};

class PowerOperatorModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(PowerOperatorModel)
public:

    enum PowerOperatorRole 
    {
        KeyRole = Qt::UserRole + 1,
        TextRole,
        VisibleRole,
        EnableRole,
    };
    Q_ENUM(PowerOperatorRole)

    PowerOperatorModel(QObject *parent = nullptr);
    ~PowerOperatorModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent= QModelIndex()) const override;;
    void setEnable(int index, bool enable);
    void setVisible(int index, bool visible);

    Q_INVOKABLE quint8 keyOfIndex(int rowIndex) const;
    Q_INVOKABLE int indexOfKey(quint8 key) const;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    void appendRow(PowerOperator *info);

private:
    QList<PowerOperator *> m_powerOperatorList;
};


Q_DECLARE_METATYPE(PowerOperatorModel)
#endif // POWEROPERATORMODEL_H