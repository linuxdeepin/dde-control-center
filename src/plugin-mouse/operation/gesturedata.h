//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GESTUREDATA_H
#define GESTUREDATA_H

#include <QObject>

class GestureData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString actionType READ actionType WRITE setActionType NOTIFY actionTypeChanged FINAL)
    Q_PROPERTY(QString direction READ direction WRITE setDirection NOTIFY directionChanged FINAL)
    Q_PROPERTY(int fingersNum READ fingersNum WRITE setFingersNum NOTIFY fingersNumChanged FINAL)
    Q_PROPERTY(QString actionName READ actionName WRITE setActionName NOTIFY actionNameChanged FINAL)

public:
    explicit GestureData(QObject *parent = nullptr);

    QString actionType() const;
    void setActionType(const QString &newActionType);

    QString direction() const;
    void setDirection(const QString &newDirection);

    int fingersNum() const;
    void setFingersNum(int newFingersNum);

    QString actionName() const;
    void setActionName(const QString &newActionName);

    QStringList actionNameList() const;
    void setActionNameList(const QStringList &newActionNameList);

    QStringList actionDescriptionList() const;
    void setActionDescriptionList(const QStringList &newActionDescriptionList);

    QList<QPair<QString, QString> > actionMaps() const;
    void setActionMaps(const QList<QPair<QString, QString> > &newActionMaps);

    void addActiosPair(const QPair<QString, QString> &actionPair);

    QString getActionFromActionDec(QString actionDec);

signals:

    void actionTypeChanged();

    void directionChanged();

    void fingersNumChanged();

    void actionNameChanged();

private:
    QString m_actionType;
    QString m_direction;
    int m_fingersNum;
    QString m_actionName;


    QList<QPair<QString, QString>> m_actionMaps;
    QStringList m_actionNameList;
    QStringList m_actionDescriptionList;

};

#endif // GESTUREDATA_H
