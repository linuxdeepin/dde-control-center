// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QMap>
#include <QDBusObjectPath>


class Port : public QObject
{
    Q_OBJECT
public:
    enum Direction {
        Out = 1,
        In = 2
    };

    explicit Port(QObject * parent) : QObject(parent),m_id(""), m_name(""), m_cardName(""), m_cardId(0), m_isActive(false), m_enabled(false), m_isBluetoothPort(false), m_direction(Out){}
    virtual ~Port() {}

    inline QString id() const { return m_id; }
    void setId(const QString &id);

    inline QString name() const { return m_name; }
    void setName(const QString &name);

    inline QString cardName() const { return m_cardName; }
    void setCardName(const QString &cardName);

    inline bool isActive() const { return m_isActive; }
    void setIsActive(bool isActive);

    inline Direction direction() const { return m_direction; }
    void setDirection(const Direction &direction);

    inline uint cardId() const { return m_cardId; }
    void setCardId(const uint &cardId);

    inline bool isEnabled() const { return m_enabled; }
    void setEnabled(const bool enabled);

    inline bool isBluetoothPort() const  { return m_isBluetoothPort; }
    void setIsBluetoothPort(const bool isBlue);

    Q_SIGNALS:
    void idChanged(QString id) const;
    void nameChanged(QString name) const;
    void cardNameChanged(QString name) const;
    void isInputActiveChanged(bool active) const;
    void isOutputActiveChanged(bool active) const;
    void directionChanged(Direction direction) const;
    void cardIdChanged(uint cardId) const;
    void currentPortEnabled(bool enable) const;
    void currentBluetoothPortChanged(bool isBlue) const;

private:
    QString m_id;
    QString m_name;
    QString m_cardName;
    uint m_cardId;
    bool m_isActive;
    bool m_enabled;
    bool m_isBluetoothPort;
    Direction m_direction;
};


#endif //PORT_H
