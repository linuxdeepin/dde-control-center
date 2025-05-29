// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccobject.h"

#include "dccobject_p.h"

#include <QLoggingCategory>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.object");

DccObject::Private *DccObject::Private::FromObject(const DccObject *obj)
{
    return obj ? obj->p_ptr : nullptr;
}

DccObject::Private::Private(DccObject *obj)
    : m_badge(0)
    , m_pageType(Menu)
    , m_weight(-1)
    , m_flags(0)
    , q_ptr(obj)
    , m_parent(nullptr)
    , m_currentObject(nullptr)
    , m_page(nullptr)
    , m_sectionItem(nullptr)
    , m_parentItem(nullptr)
{
}

DccObject::Private::~Private()
{
    if (m_sectionItem) {
        m_parentItem = nullptr;
        delete m_sectionItem;
        m_sectionItem = nullptr;
    }
    if (m_page && (!m_page->parent() || m_page->parent() == q_ptr)) {
        delete m_page;
        m_page = nullptr;
    }
    if (m_parent) {
        m_parent->p_ptr->removeChild(q_ptr);
    }
    while (!m_children.isEmpty()) {
        DccObject *child = m_children.first();
        removeChild(0);
        if (child->parent() == q_ptr) {
            delete child;
        }
    }
}

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
            if (!hidden) {
                deleteSectionItem();
            }
            Q_EMIT q_ptr->visibleToAppChanged(hidden);
        }
        bool allDisabled = getFlagState(DCC_ALL_DISABLED);
        if (disabled != allDisabled) {
            if (m_sectionItem) {
                m_sectionItem->setEnabled(!allDisabled);
            }
            Q_EMIT q_ptr->enabledToAppChanged(disabled);
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

bool DccObject::Private::addChild(DccObject *child, bool updateParent)
{
    if (!child) {
        return false;
    }
    int index = 0;
    for (auto &&it = m_children.cbegin(); it != m_children.cend(); it++) {
        if (*it == child)
            return false;
        if (child->weight() >= (*it)->weight()) {
            index++;
        }
    }

    Q_EMIT q_ptr->childAboutToBeAdded(q_ptr, index);
    m_children.insert(m_children.cbegin() + index, child);
    DccObject::Private::FromObject(child)->SetParent(q_ptr);
    if (updateParent) {
        child->setParent(q_ptr);
    }
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
    m_children.erase(m_children.cbegin() + index);
    DccObject::Private::FromObject(child)->SetParent(nullptr);
    if (child == m_currentObject) {
        q_ptr->setCurrentObject(nullptr);
    }
    Q_EMIT q_ptr->childRemoved(child);
    Q_EMIT q_ptr->childrenChanged(m_children);
}

void DccObject::Private::removeChild(DccObject *child)
{
    int pos = getChildIndex(child);
    if (pos >= 0)
        removeChild(pos);
}

void DccObject::Private::updatePos(DccObject *child)
{
    int oldPos = -1;
    int modelPos = 0;
    int index = 0;
    for (auto &&it = m_children.cbegin(); it != m_children.cend(); it++, index++) {
        if (*it == child) {
            oldPos = index;
        } else if (child->weight() >= (*it)->weight()) {
            modelPos = index + 1;
        }
    }

    int newPos = oldPos < modelPos ? modelPos - 1 : modelPos;
    if (oldPos < 0 || oldPos == newPos) {
        return;
    }
    Q_EMIT q_ptr->childAboutToBeMoved(q_ptr, modelPos, oldPos);
    m_children.move(oldPos, newPos);
    Q_EMIT q_ptr->childMoved(child);
    Q_EMIT q_ptr->childrenChanged(m_children);
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

void DccObject::Private::deleteSectionItem()
{
    if (m_sectionItem) {
        QQuickItem *item = m_sectionItem.get();
        m_sectionItem = nullptr;
        q_ptr->setParentItem(nullptr);
        for (auto &&child : m_children) {
            if (child->p_ptr->m_sectionItem) {
                Q_EMIT child->deactive();
            }
        }
        // 延时delete等动画完成
        QTimer::singleShot(500, item, [item]() {
            item->deleteLater();
        });
    }
}

void DccObject::Private::data_append(QQmlListProperty<QObject> *data, QObject *o)
{
    if (!o)
        return;
    DccObject::Private *that = reinterpret_cast<DccObject::Private *>(data->data);
    that->m_data.append(o);
    o->setParent(that->q_ptr);

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
    connect(this, &DccObject::deactive, this, [this]() {
        p_ptr->deleteSectionItem();
    });
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

quint32 DccObject::weight() const
{
    return p_ptr->m_weight;
}

void DccObject::setWeight(quint32 weight)
{
    if (p_ptr->m_weight != weight) {
        p_ptr->m_weight = weight;
        if (p_ptr->m_parent) {
            p_ptr->m_parent->p_ptr->updatePos(this);
        }
        Q_EMIT weightChanged(p_ptr->m_weight);
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
        if (!icon.isEmpty()) {
            QQmlContext *context = qmlContext(this);
            p_ptr->m_iconSource = context ? context->resolvedUrl(icon) : icon;
        } else {
            p_ptr->m_iconSource.clear();
        }
        Q_EMIT iconChanged(p_ptr->m_icon);
        Q_EMIT iconSourceChanged(p_ptr->m_iconSource);
    }
}

QUrl DccObject::iconSource() const
{
    return p_ptr->m_iconSource;
}

qint8 DccObject::badge() const
{
    return p_ptr->m_badge;
}

void DccObject::setBadge(qint8 badge)
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

bool DccObject::canSearch() const
{
    return !p_ptr->getFlagState(DCC_CANSEARCH);
}

void DccObject::setCanSearch(bool canSearch)
{
    if (p_ptr->setFlagState(DCC_CANSEARCH, !canSearch)) {
        Q_EMIT canSearchChanged(canSearch);
    }
}

DccObject::BackgroundTypes DccObject::backgroundType() const
{
    return BackgroundTypes(p_ptr->m_flags & DCC_MASK_BGTYPE);
}

void DccObject::setBackgroundType(BackgroundTypes type)
{
    if (backgroundType() != type) {
        p_ptr->m_flags &= DCC_MASK_NOBGTYPE;
        p_ptr->m_flags |= type;
        Q_EMIT backgroundTypeChanged(type);
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

quint8 DccObject::pageType() const
{
    return p_ptr->m_pageType;
}

void DccObject::setPageType(quint8 type)
{
    if (p_ptr->m_pageType != type) {
        p_ptr->m_pageType = type;
        Q_EMIT pageTypeChanged(p_ptr->m_pageType);
    }
}

QQuickItem *DccObject::getSectionItem(QObject *)
{
    p_ptr->deleteSectionItem();
    if (p_ptr->m_page) {
        QQmlContext *creationContext = p_ptr->m_page->creationContext();
        QQmlContext *context = new QQmlContext(creationContext, p_ptr->m_page);
        context->setContextProperty("dccObj", this);
#if 0
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
                    p_ptr->m_sectionItem->setParent(this);
                } else if (QQuickWindow *p = qobject_cast<QQuickWindow *>(parent)) {
                    p_ptr->m_sectionItem->setParent(this);
                } else {
                    p_ptr->m_sectionItem->setParent(this);
                }
                p_ptr->m_sectionItem->setEnabled(isEnabledToApp());
            }
            // sections are not controlled by FxListItemSG, so apply attached properties here
        } else {
            qCWarning(dccLog()) << "create page error:" << p_ptr->m_page->errorString();
            delete context;
        }
        p_ptr->m_page->completeCreate();
#else
        p_ptr->m_sectionItem = qobject_cast<QQuickItem *>(p_ptr->m_page->create(context));
        if (p_ptr->m_sectionItem) {
            p_ptr->m_sectionItem->setParent(this);
            p_ptr->m_sectionItem->setEnabled(isEnabledToApp());
        } else {
            qCWarning(dccLog()) << "create page error:" << p_ptr->m_page->errorString();
            delete context;
        }
#endif
    }
    return p_ptr->m_sectionItem.get();
}

QQuickItem *DccObject::parentItem()
{
    return p_ptr->m_parentItem.get();
}

void DccObject::setParentItem(QQuickItem *item)
{
    if (item != p_ptr->m_parentItem.get()) {
        p_ptr->m_parentItem = item;
        Q_EMIT parentItemChanged(p_ptr->m_parentItem.get());
    }
}

QQmlComponent *DccObject::page() const
{
    return p_ptr->m_page;
}

void DccObject::setPage(QQmlComponent *page)
{
    if (p_ptr->m_page.get() != page) {
        p_ptr->m_page = page;
        if (p_ptr->m_page && !p_ptr->m_page->parent()) {
            p_ptr->m_page->setParent(this);
        }
        Q_EMIT pageChanged(p_ptr->m_page.get());
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

} // namespace dccV25
