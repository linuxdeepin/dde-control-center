// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCOBJECT_P_H
#define DCCOBJECT_P_H

#include "dccobject.h"

#include <QVector>
#define DCC_HIDDEN 0x80000000
#define DCC_DISABLED 0x40000000
#define DCC_CONFIG_HIDDEN 0x20000000
#define DCC_CONFIG_DISABLED 0x10000000
// 0xA0000000 = 0x80000000|0x20000000 0x80000000为用户设置位 0x20000000为配置设置位
// 0x50000000 = 0x40000000|0x10000000 0x4000000为用户设置位 0x10000000为配置设置位
#define DCC_ALL_HIDDEN 0xA0000000
#define DCC_ALL_DISABLED 0x50000000

#define DCC_CUSTOM_DEFULT 0x08000000

#define DCC_LABELHIDDEN 0x00008000   // 扩展按钮(子项)
#define DCC_HASBACKGROUND 0x00004000 // 扩展按钮(子项)
#define DCC_EXTRA 0x00800000         // 扩展按钮(子项)

#define DCC_EXTRA 0x00800000     // 扩展按钮(子项)
#define DCC_CANSEARCH 0x04000000 // 不参与搜索

namespace dccV25 {
class DccObject::Private
{
public:
    static DccObject::Private *FromObject(const DccObject *obj);

    bool getFlagState(uint32_t flag) const;
    bool setFlagState(uint32_t flag, bool state);
    uint32_t getFlag() const;

    bool addChild(DccObject::Private *child);
    bool addChild(DccObject *child);
    void removeChild(int index);
    void removeChild(DccObject *child);
    void updatePos(DccObject *child);
    const QVector<DccObject *> &getChildren() const;
    int getIndex() const;

    inline DccObject *getParent() const { return m_parent; }

    inline const QVector<DccObject *> &getObjects() const { return m_objects; }

    DccObject *getChild(int childPos) const;
    int getChildIndex(const DccObject *child) const;

protected:
    explicit Private(DccObject *obj);
    virtual ~Private();

    virtual inline void SetParent(DccObject *anObject) { m_parent = anObject; }

private:
    // data property
    static void data_append(QQmlListProperty<QObject> *data, QObject *o);
    static qsizetype data_count(QQmlListProperty<QObject> *data);
    static QObject *data_at(QQmlListProperty<QObject> *data, qsizetype i);
    static void data_clear(QQmlListProperty<QObject> *data);

protected:
    DccObject *q_ptr;
    DccObject *m_parent;
    DccObject *m_currentObject;
    DccObject *m_defultObject;
    QVector<DccObject *> m_children;
    QVector<DccObject *> m_objects;
    QObjectList m_data;
    QPointer<QQmlComponent> m_page;
    QPointer<QQuickItem> m_sectionItem;
    QPointer<QQuickItem> m_parentItem;

    QString m_parentName;
    QString m_displayName;
    QString m_description;
    QString m_icon;
    QUrl m_iconSource;
    int m_weight;
    int m_badge;
    uint m_pageType;

    uint32_t m_flags;
    friend class DccObject;
};
} // namespace dccV25
#endif // DCCOBJECT_P_H
