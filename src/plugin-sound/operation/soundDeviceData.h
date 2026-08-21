// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SOUNDDEVICEDATA_H
#define SOUNDDEVICEDATA_H

#include <QString>
#include "port.h"

class SoundDeviceData {


public:
    explicit SoundDeviceData();

    bool ischecked() const;
    void setIschecked(bool newIschecked);

    QString name() const;
    void setName(const QString &newName);

    QString getPortId() const;
    void setPortId(const QString &newPortId);

    uint getCardId() const;
    void setCardId(uint newCardId);

    Port *port() const;
    void setPort(Port *newPort);

private:
    bool m_ischecked;
    QString m_name;

    QString portId;
    uint cardId;
};



#endif //SOUNDDEVICEDATA_H
