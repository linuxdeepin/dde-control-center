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

#define DCC_MASK_BGTYPE 0x000000FF   // 背景类型
#define DCC_MASK_NOBGTYPE 0xFFFFFF00 // 背景类型

#define DCC_CUSTOM_DEFULT 0x08000000

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
    bool addChild(DccObject *child, bool updateParent = true);
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
    qint8 m_badge;
    quint8 m_pageType;
    quint16 m_weight;
    quint32 m_flags;

    DccObject *q_ptr;    // q指针
    DccObject *m_parent; // 父项
    DccObject *m_currentObject;
    QVector<DccObject *> m_children; // 子项
    QVector<DccObject *> m_objects;  // m_data中DccObject(未保证有效(delete时未处理))
    QObjectList m_data;              // data属性，为qml能加子项
    QPointer<QQmlComponent> m_page;
    QPointer<QQuickItem> m_parentItem;  // Item父项

    QString m_parentName;
    QString m_displayName;
    QString m_description;
    QString m_icon;    // 属性
    QUrl m_iconSource; // icon带相对路径处理
    friend class DccObject;
};
} // namespace dccV25
#endif // DCCOBJECT_P_H
