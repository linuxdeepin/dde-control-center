// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TOUCHSCREENINFOLIST_H
#define TOUCHSCREENINFOLIST_H

#include <QString>
#include <QList>
#include <QDBusMetaType>

struct TouchscreenInfo {
    qint32 id;
    QString name;
    QString deviceNode;
    QString serialNumber;

    bool operator ==(const TouchscreenInfo& info) const;
    bool operator !=(const TouchscreenInfo& info) const;
};

typedef QList<TouchscreenInfo> TouchscreenInfoList;

// 自定义 QList<TouchscreenInfo_V2> 的 != 操作符
inline bool operator!=(const TouchscreenInfoList& list1, const TouchscreenInfoList& list2) {
    if (list1.size() != list2.size())
        return true;

    for (int i = 0; i < list1.size(); ++i) {
        if (list1[i] != list2[i]) {
            return true;
        }
    }
    return false;
}

Q_DECLARE_METATYPE(TouchscreenInfo)
Q_DECLARE_METATYPE(TouchscreenInfoList)

QDBusArgument &operator<<(QDBusArgument &arg, const TouchscreenInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &arg, TouchscreenInfo &info);

void registerTouchscreenInfoListMetaType();

#endif // !TOUCHSCREENINFOLIST_H
