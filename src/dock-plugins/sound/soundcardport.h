
// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDCARDPORT_H
#define SOUNDCARDPORT_H

#include <QObject>

class SoundCardPort : public QObject {
    Q_OBJECT
public:
    enum Direction {
        Out = 1,
        In = 2
    };

    enum PortType {
        Bluetooth,
        Headphone,
        Usb,
        Builtin,
        HDMI,
        LineIO,
        MultiChannel,
        Other
    };

    explicit SoundCardPort(QObject* parent = nullptr);
    virtual ~SoundCardPort() { }

    inline QString portName() const { return m_portName; }
    void setPortName(const QString& name);

    inline QString description() const { return m_description; }
    void setDescription(const QString& description);

    inline QString cardName() const { return m_cardName; }
    void setCardName(const QString& cardName);

    inline bool isActive() const { return m_active; }
    void setActive(bool active);

    inline Direction direction() const { return m_direction; }
    void setDirection(const Direction& direction);

    inline uint cardId() const { return m_cardId; }
    void setCardId(const uint& cardId);

    void setEnabled(bool enabled);
    inline bool isEnabled() const { return m_enabled; }

    void setBluetooth(bool bluetooth);
    inline bool isBluetooth() const { return m_bluetooth; }

    void setPortType(int type);
    inline PortType portType() const { return m_portType; }

    inline QString uniqueKey() const { return compositeKey(m_cardId, m_portName); }

    bool operator==(const SoundCardPort& port) const { return m_portName == port.portName() && m_cardId == port.cardId(); }

    // 分隔字符没有特殊意义，只是为了防重复
    static QString compositeKey(uint cardId, const QString& portName) { return QString::number(cardId) + "_dock-separate-key_" + portName; }
    static QStringList decomposeKey(const QString& key) { return key.split("_dock-separate-key_"); }

    static QString icon(PortType type)
    {
        switch (type) {
            case Builtin:
                return "sound_speaker";
            case Headphone:
                return "sound_headphone";
            case HDMI:
                return "sound_hdmi";
            case Bluetooth:
                return "sound_bluetooth";
            case Other:
                return "sound_other";
            default:
                return "sound_other";
        }
    }

Q_SIGNALS:
    void idChanged(QString id) const;
    void nameChanged(QString name) const;
    void cardNameChanged(QString name) const;
    void activityChanged(bool active) const;
    void directionChanged(Direction direction) const;
    void cardIdChanged(uint cardId) const;
    void enabledChanged(bool enabled);

private:
    QString m_portName;
    QString m_description;
    uint m_cardId;
    QString m_cardName;
    bool m_active;
    bool m_enabled;
    bool m_bluetooth;
    Direction m_direction;
    PortType m_portType;
};

Q_DECLARE_METATYPE(const SoundCardPort*)

#endif