//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GESTUREDATA_H
#define GESTUREDATA_H

#include <QObject>

struct GestureActionData
{
    QString actionId;
    QString displayName;
    bool supported = false;
    QString unavailableReason;
};

class GestureData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString actionType READ actionType WRITE setActionType NOTIFY actionTypeChanged FINAL)
    Q_PROPERTY(QString gestureId READ gestureId WRITE setGestureId NOTIFY gestureIdChanged FINAL)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged FINAL)
    Q_PROPERTY(QString direction READ direction WRITE setDirection NOTIFY directionChanged FINAL)
    Q_PROPERTY(int fingersNum READ fingersNum WRITE setFingersNum NOTIFY fingersNumChanged FINAL)
    Q_PROPERTY(QString actionName READ actionName WRITE setActionName NOTIFY actionNameChanged FINAL)
    Q_PROPERTY(int sequence READ sequence WRITE setSequence NOTIFY sequenceChanged FINAL)

public:
    explicit GestureData(QObject *parent = nullptr);

    QString actionType() const;
    void setActionType(const QString &newActionType);

    QString gestureId() const;
    void setGestureId(const QString &newGestureId);

    QString displayName() const;
    void setDisplayName(const QString &newDisplayName);

    QString direction() const;
    void setDirection(const QString &newDirection);

    int fingersNum() const;
    void setFingersNum(int newFingersNum);

    QString actionName() const;
    void setActionName(const QString &newActionName);

    int sequence() const;
    void setSequence(int newSequence);

    QList<GestureActionData> actions() const;
    void setActions(const QList<GestureActionData> &actions);

signals:

    void actionTypeChanged();

    void gestureIdChanged();

    void displayNameChanged();

    void directionChanged();

    void fingersNumChanged();

    void actionNameChanged();

    void sequenceChanged();

private:
    QString m_actionType;
    QString m_gestureId;
    QString m_displayName;
    QString m_direction;
    int m_fingersNum;
    QString m_actionName;
    int m_sequence = -1;


    QList<GestureActionData> m_actions;
};

Q_DECLARE_METATYPE(GestureActionData)
Q_DECLARE_METATYPE(QList<GestureActionData>)

#endif // GESTUREDATA_H
