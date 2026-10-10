// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TOUCHSCREENINFOLISTV2_H
#define TOUCHSCREENINFOLISTV2_H

#include <QString>
#include <QList>
#include <QDBusMetaType>

struct TouchscreenInfo_V2 {
    qint32 id;
    QString name;
    QString deviceNode;
    QString serialNumber;
    QString UUID;

    bool operator ==(const TouchscreenInfo_V2& info) const;
    bool operator !=(const TouchscreenInfo_V2& info) const;
};

typedef QList<TouchscreenInfo_V2> TouchscreenInfoList_V2;

inline bool operator!=(const TouchscreenInfoList_V2 &list1, const TouchscreenInfoList_V2 &list2) {
    if (list1.size() != list2.size())
        return true;

    for (int i = 0; i < list1.size(); ++i) {
        if (list1[i] != list2[i]) {
            return true;
        }
    }
    return false;
}

Q_DECLARE_METATYPE(TouchscreenInfo_V2)
Q_DECLARE_METATYPE(TouchscreenInfoList_V2)

QDBusArgument &operator<<(QDBusArgument &arg, const TouchscreenInfo_V2 &info);
const QDBusArgument &operator>>(const QDBusArgument &arg, TouchscreenInfo_V2 &info);

void registerTouchscreenInfoList_V2MetaType();

#endif // !TOUCHSCREENINFOLISTV2_H
