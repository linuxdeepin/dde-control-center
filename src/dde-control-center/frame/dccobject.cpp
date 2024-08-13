// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccobject.h"

#include "dccobject_p.h"

#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>

namespace dccV25 {
DccObject::Private *DccObject::Private::FromObject(const DccObject *obj)
{
    return obj ? obj->p_ptr : nullptr;
}

DccObject::Private::Private(DccObject *obj)
    : q_ptr(obj)
    , m_parent(nullptr)
    , m_currentObject(nullptr)
    , m_defultObject(nullptr)
    , m_page(nullptr)
    , m_sectionItem(nullptr)
    , m_weight(-1)
    , m_badge(0)
    , m_pageType(0)
    , m_flags(0)
{
}

DccObject::Private::~Private() { }

bool DccObject::Private::getFlagState(uint32_t flag) const
{
    return (m_flags & flag);
}

bool DccObject::Private::setFlagState(uint32_t flag, bool state)
{
    if (getFlagState(flag) != state) {
        bool hidden = getFlagState(DCC_ALL_HIDDEN);
        bool disabled = getFlagState(DCC_ALL_DISABLED);
        if (state)
            m_flags |= flag;
        else
            m_flags &= (~flag);
        if (hidden != getFlagState(DCC_ALL_HIDDEN)) {
            Q_EMIT q_ptr->visibleToAppChanged(hidden);
        }
        if (disabled != getFlagState(DCC_ALL_DISABLED)) {
            Q_EMIT q_ptr->enabledChanged(disabled);
        }
        return true;
    }
    return false;
}

uint32_t DccObject::Private::getFlag() const
{
    return m_flags;
}

bool DccObject::Private::addChild(DccObject::Private *child)
{
    return addChild(child->q_ptr);
}

bool DccObject::Private::addChild(DccObject *child)
{
    if (!child) {
        return false;
    }
    int index = 0;
    for (auto &&it = m_children.begin(); it != m_children.end(); it++) {
        if (*it == child)
            return false;
        if (child->weight() >= (*it)->weight()) {
            index++;
        }
    }

    Q_EMIT q_ptr->childAboutToBeAdded(q_ptr, index);
    m_children.insert(m_children.begin() + index, child);
    DccObject::Private::FromObject(child)->SetParent(q_ptr);
    child->setParent(q_ptr);
    Q_EMIT q_ptr->childAdded(child);
    Q_EMIT q_ptr->childrenChanged(m_children);
    return true;
}

void DccObject::Private::removeChild(int index)
{
    if (index < 0 || index >= m_children.size()) {
        // Q_ASSERT(false);
        return;
    }

    DccObject *child = m_children.at(index);
    Q_EMIT q_ptr->childAboutToBeRemoved(q_ptr, index);
    // we can't swap as we want to keep the order!
    //(DGM: do this BEFORE deleting the object (otherwise
    // the dependency mechanism can 'backfire' ;)
    m_children.erase(m_children.begin() + index);
    DccObject::Private::FromObject(child)->SetParent(nullptr);
    child->setParent(nullptr);
    Q_EMIT q_ptr->childRemoved(child);
    Q_EMIT q_ptr->childrenChanged(m_children);
}

void DccObject::Private::removeChild(DccObject *child)
{
    int pos = getChildIndex(child);
    if (pos >= 0)
        removeChild(pos);
}

const QVector<DccObject *> &DccObject::Private::getChildren() const
{
    return m_children;
}

int DccObject::Private::getIndex() const
{
    return m_parent ? m_parent->p_ptr->getChildren().indexOf(q_ptr) : -1;
}

DccObject *DccObject::Private::getChild(int childPos) const
{
    return (childPos >= 0 && childPos < m_children.size() ? m_children[childPos] : nullptr);
}

int DccObject::Private::getChildIndex(const DccObject *child) const
{
    return m_children.indexOf(const_cast<DccObject *>(child));
}

void DccObject::Private::data_append(QQmlListProperty<QObject> *data, QObject *o)
{
    if (!o)
        return;
    DccObject::Private *that = reinterpret_cast<DccObject::Private *>(data->data);
    that->m_data.append(o);

    if (DccObject *obj = qobject_cast<DccObject *>(o)) {
        DccObject::Private *parent = reinterpret_cast<DccObject::Private *>(data->data);
        parent->m_objects.append(obj);
    }
}

qsizetype DccObject::Private::data_count(QQmlListProperty<QObject> *data)
{
    return reinterpret_cast<DccObject::Private *>(data->data)->m_data.count();
}

QObject *DccObject::Private::data_at(QQmlListProperty<QObject> *data, qsizetype i)
{
    QObjectList &d = reinterpret_cast<DccObject::Private *>(data->data)->m_data;
    if (i < 0 || i >= d.size())
        return nullptr;
    return d.at(i);
}

void DccObject::Private::data_clear(QQmlListProperty<QObject> *data)
{
    reinterpret_cast<DccObject::Private *>(data->data)->m_data.clear();
    reinterpret_cast<DccObject::Private *>(data->data)->m_objects.clear();
}

///////////////////////////////////////////////////////
DccObject::DccObject(QObject *parent)
    : QObject(parent)
    , p_ptr(new DccObject::Private(this))
{
}

DccObject::~DccObject()
{
    delete p_ptr;
}

QString DccObject::name() const
{
    return objectName();
}

void DccObject::setName(const QString &name)
{
    setObjectName(name);
    Q_EMIT nameChanged(name);
}

QString DccObject::parentName() const
{
    return p_ptr->m_parentName;
}

void DccObject::setParentName(const QString &parentName)
{
    if (p_ptr->m_parentName != parentName) {
        p_ptr->m_parentName = parentName;
        Q_EMIT parentNameChanged(p_ptr->m_parentName);
    }
}

int DccObject::weight() const
{
    return p_ptr->m_weight;
}

void DccObject::setWeight(int weight)
{
    if (p_ptr->m_weight != weight) {
        p_ptr->m_weight = weight;
    }
}

QString DccObject::displayName() const
{
    return p_ptr->m_displayName;
}

void DccObject::setDisplayName(const QString &displayName)
{
    if (p_ptr->m_displayName != displayName) {
        p_ptr->m_displayName = displayName;
        Q_EMIT displayNameChanged(p_ptr->m_displayName);
    }
}

QString DccObject::description() const
{
    return p_ptr->m_description;
}

void DccObject::setDescription(const QString &description)
{
    if (p_ptr->m_description != description) {
        p_ptr->m_description = description;
        Q_EMIT descriptionChanged(p_ptr->m_description);
    }
}

QString DccObject::icon() const
{
    return p_ptr->m_icon;
}

void DccObject::setIcon(const QString &icon)
{
    if (p_ptr->m_icon != icon) {
        p_ptr->m_icon = icon;
        Q_EMIT iconChanged(p_ptr->m_icon);
    }
}

int DccObject::badge() const
{
    return p_ptr->m_badge;
}

void DccObject::setBadge(int badge)
{
    if (p_ptr->m_badge != badge) {
        p_ptr->m_badge = badge;
        Q_EMIT badgeChanged(p_ptr->m_badge);
    }
}

bool DccObject::isVisible() const
{
    return !p_ptr->getFlagState(DCC_HIDDEN);
}

void DccObject::setVisible(bool isVisible)
{
    if (p_ptr->setFlagState(DCC_HIDDEN, !isVisible)) {
        Q_EMIT visibleChanged(isVisible);
    }
}

bool DccObject::isVisibleToApp() const
{
    return !p_ptr->getFlagState(DCC_ALL_HIDDEN);
}

bool DccObject::isEnabled() const
{
    return !p_ptr->getFlagState(DCC_DISABLED);
}

void DccObject::setEnabled(bool enabled)
{
    if (p_ptr->setFlagState(DCC_DISABLED, !enabled)) {
        Q_EMIT enabledChanged(enabled);
    }
}

bool DccObject::isEnabledToApp() const
{
    return !p_ptr->getFlagState(DCC_ALL_DISABLED);
}

bool DccObject::hasBackground() const
{
    return p_ptr->getFlagState(DCC_HASBACKGROUND);
}

void DccObject::setHasBackground(bool hasBackground)
{
    if (p_ptr->setFlagState(DCC_HASBACKGROUND, hasBackground)) {
        Q_EMIT hasBackgroundChanged(hasBackground);
    }
}

DccObject *DccObject::currentObject()
{
    return p_ptr->m_currentObject;
}

void DccObject::setCurrentObject(DccObject *obj)
{
    if (p_ptr->m_currentObject != obj) {
        if (p_ptr->m_currentObject) {
            Q_EMIT p_ptr->m_currentObject->deactive();
        }
        p_ptr->m_currentObject = obj;
        Q_EMIT currentObjectChanged(p_ptr->m_currentObject);
    }
}

DccObject *DccObject::defultObject()
{
    if (p_ptr->getFlagState(DCC_CUSTOM_DEFULT)) {
        return p_ptr->m_defultObject;
    } else {
        return p_ptr->getChild(0);
    }
}

void DccObject::setDefultObject(DccObject *obj)
{
    if (p_ptr->setFlagState(DCC_CUSTOM_DEFULT, true) || p_ptr->m_defultObject != obj) {
        p_ptr->m_defultObject = obj;
        Q_EMIT defultObjectChanged(p_ptr->m_defultObject);
    }
}

uint DccObject::pageType() const
{
    return p_ptr->m_pageType;
}

void DccObject::setPageType(uint type)
{
    if (p_ptr->m_pageType != type) {
        p_ptr->m_pageType = type;
        Q_EMIT pageTypeChanged(p_ptr->m_pageType);
    }
}

QQuickItem *DccObject::getSectionItem(QObject *parent)
{
    if (p_ptr->m_sectionItem)
        return p_ptr->m_sectionItem;
    if (p_ptr->m_page) {
        QQmlContext *creationContext = p_ptr->m_page->creationContext();
        QQmlContext *context = new QQmlContext(creationContext);
        context->setContextProperty("dccObj", this);
        QObject *nobj = p_ptr->m_page->beginCreate(context);
        if (nobj) {
            p_ptr->m_sectionItem = qobject_cast<QQuickItem *>(nobj);
            if (!p_ptr->m_sectionItem) {
                delete nobj;
            } else {
                if (qFuzzyIsNull(p_ptr->m_sectionItem->z()))
                    p_ptr->m_sectionItem->setZ(2);
                // QQml_setParent_noEvent(sectionItem, contentItem);
                if (QQuickItem *p = qobject_cast<QQuickItem *>(parent)) {
                    p_ptr->m_sectionItem->setParentItem(p);
                    p_ptr->m_sectionItem->setParent(p);
                } else if (QQuickWindow *p = qobject_cast<QQuickWindow *>(parent)) {
                    p_ptr->m_sectionItem->setParent(p);
                } else {
                    p_ptr->m_sectionItem->setParent(this);
                }
            }
            // sections are not controlled by FxListItemSG, so apply attached properties here
        } else {
            delete context;
        }
        p_ptr->m_page->completeCreate();
    }
    return p_ptr->m_sectionItem;
}

QQmlComponent *DccObject::page() const
{
    return p_ptr->m_page;
}

void DccObject::setPage(QQmlComponent *page)
{
    if (p_ptr->m_page != page) {
        p_ptr->m_page = page;
        Q_EMIT pageChanged(p_ptr->m_page);
    }
}

QQmlListProperty<QObject> DccObject::data()
{
    return QQmlListProperty<QObject>(this, p_ptr, &DccObject::Private::data_append, &DccObject::Private::data_count, &DccObject::Private::data_at, &DccObject::Private::data_clear);
}

const QVector<DccObject *> &DccObject::getChildren() const
{
    return p_ptr->getChildren();
}

void DccObject::trigger()
{
    Q_EMIT triggered();
}
} // namespace dccV25
