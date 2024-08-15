//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/moduleobject.h"

#include <QWidget>

#define DCC_HIDDEN 0x80000000
#define DCC_DISABLED 0x40000000

// 0xA0000000 = 0x80000000|0x20000000 0x80000000为用户设置位 0x20000000为配置设置位
// 0x50000000 = 0x40000000|0x10000000 0x4000000为用户设置位 0x10000000为配置设置位
#define DCC_ALL_HIDDEN 0xA0000000
#define DCC_ALL_DISABLED 0x50000000

#define DCC_EXTRA 0x00800000 // 扩展按钮(子项)
#define DCC_NOSEARCH 0x08000000 // 不参与搜索

/** Versions:
    V1.0 - 2023/05/24 - create
**/
const unsigned c_currentVersion = 10; // 1.0

// DCORE_USE_NAMESPACE

using namespace DCC_NAMESPACE;
namespace DCC_NAMESPACE {

class ModuleObjectPrivate
{
public:
    explicit ModuleObjectPrivate(ModuleObject *object)
        : q_ptr(object)
        , m_currentModule(nullptr)
        , m_badge(0)
        , m_flags(0)
    {
    }
    QString modelDescription() {
        Q_Q(ModuleObject);
        // The punctuation symbol used to separate items in a list. e.g. A, B, C, D"
        const QString SPLIT_CHAR = QObject::tr(", ");
        QString description;
        for (const auto child : q->childrens()) {
            if (child->isHidden())
                continue;
            const auto &name = child->displayName();
            if (!name.isEmpty())
                description.append(QString("%1%2").arg(name).arg(SPLIT_CHAR));
        }
        description.chop(SPLIT_CHAR.size());
        if (!description.isEmpty())
            return description;
        return q->displayName();
    }
public:
    ModuleObject *q_ptr;
    Q_DECLARE_PUBLIC(ModuleObject)

    QList<ModuleObject *> m_childrens;
    ModuleObject *m_currentModule;

    QString m_name;                // 名称，作为每个模块的唯一标识，不可为空
    QString m_displayName;         // 显示名称，如菜单的名称，页面的标题等，为空则不显示
    QString m_fallbackDescription; // 描述，如主菜单的描述信息，如果m_description为空，该值会跟随插件变换
    QString m_description;         // 主动设置的的描述信息，当非空，为显示的描述信息
    QStringList m_contentText;     // 上下文数据，参与搜索，只可用于终结点：DisplayName -> ContentText(one of it)
    QVariant m_icon;               // 图标，如主菜单的图标
    int m_badge;                   // 主菜单中的角标, 默认为0不显示，大于0显示

    uint32_t m_flags;
};
}
ModuleObject::ModuleObject(QObject *parent)
    : ModuleObject(QString(), QString(), parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, QStringList(), parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, {}, contentText, parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, {}, contentText, QVariant(), parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, {}, icon, parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QString &description, QObject *parent)
    : ModuleObject(name, displayName, description, {}, QVariant(), parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QString &description, const QVariant &icon, QObject *parent)
    : ModuleObject(name, displayName, description, {}, icon, parent)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, {}, QVariant::fromValue(icon), parent)
{
}

ModuleObject::ModuleObject(const ModuleInitContext &message, QObject *parent)
    : ModuleObject(message.name, message.displayName, message.description, message.contentText, message.icon, parent)
{

}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QVariant &icon, QObject *parent)
    : QObject(parent)
    , DCC_INIT_PRIVATE(ModuleObject)
{
    Q_D(ModuleObject);
    d->m_name = name;
    d->m_displayName = displayName;
    d->m_description = description;
    d->m_contentText = contentText;
    d->m_icon = icon;
    d->m_fallbackDescription = d->modelDescription();
    if (description.isEmpty()) {
        connect(this, &ModuleObject::insertedChild, this, [this](ModuleObject *) {
            Q_D(ModuleObject);
            d->m_fallbackDescription = d->modelDescription();
            Q_EMIT moduleDataChanged();
        });
        connect(this, &ModuleObject::removedChild, this, [this](ModuleObject *) {
            Q_D(ModuleObject);
            d->m_fallbackDescription = d->modelDescription();
            Q_EMIT moduleDataChanged();
        });
    }
}

ModuleObject::~ModuleObject()
{
    deactive();
}

QWidget *ModuleObject::activePage(bool autoActive)
{
    if (autoActive)
        active();
    QWidget *w = ModuleObject::IsHidden(this) ? nullptr : page();
    // 处理page中修改隐藏状态
    if (w && ModuleObject::IsHidden(this)) {
        delete w;
        w = nullptr;
    }

    if (w) {
        connect(w, &QObject::destroyed, this, &ModuleObject::deactive);
        connect(this, &ModuleObject::stateChanged, w, [w](uint32_t flag, bool state) {
            if (ModuleObject::IsDisabledFlag(flag))
                w->setDisabled(state);
        });
        w->setDisabled(ModuleObject::IsDisabled(this));
    } else {
        deactive();
    }
    return w;
}

void ModuleObject::active()
{
}

QWidget *ModuleObject::page()
{
    return nullptr;
}

void ModuleObject::deactive()
{
}

QString ModuleObject::name() const
{
    Q_D(const ModuleObject);
    return d->m_name;
}
QString ModuleObject::displayName() const
{
    Q_D(const ModuleObject);
    return d->m_displayName;
}
QString ModuleObject::description() const
{
    Q_D(const ModuleObject);
    if (d->m_description.isEmpty()) {
        return d->m_fallbackDescription;
    } else {
        return d->m_description;
    }

}
QStringList ModuleObject::contentText() const
{
    Q_D(const ModuleObject);
    return d->m_contentText;
}
QVariant ModuleObject::icon() const
{
    Q_D(const ModuleObject);
    return d->m_icon;
}
int ModuleObject::badge() const
{
    Q_D(const ModuleObject);
    return d->m_badge;
}

bool ModuleObject::isHidden() const
{
    return getFlagState(DCC_HIDDEN);
}

bool ModuleObject::isVisible() const
{
    return !isHidden();
}

bool ModuleObject::isDisabled() const
{
    return getFlagState(DCC_DISABLED);
}

bool ModuleObject::isEnabled() const
{
    return !isDisabled();
}

unsigned ModuleObject::GetCurrentVersion()
{
    return c_currentVersion;
}

void ModuleObject::setHidden(bool hidden)
{
    setFlagState(DCC_HIDDEN, hidden);
    Q_EMIT visibleChanged();
}

void ModuleObject::setVisible(bool visible)
{
    setHidden(!visible);
}

void ModuleObject::setDisabled(bool disabled)
{
    setFlagState(DCC_DISABLED, disabled);
}

void ModuleObject::setEnabled(bool enabled)
{
    setDisabled(!enabled);
}

void ModuleObject::trigger()
{
    Q_EMIT triggered();
}

void ModuleObject::setName(const QString &name)
{
    Q_D(ModuleObject);
    if (d->m_name != name) {
        d->m_name = name;
        Q_EMIT moduleDataChanged();
    }
}
void ModuleObject::setDisplayName(const QString &displayName)
{
    Q_D(ModuleObject);
    if (d->m_displayName != displayName) {
        d->m_displayName = displayName;
        Q_EMIT displayNameChanged(d->m_displayName);
        Q_EMIT moduleDataChanged();
    }
}
void ModuleObject::setDescription(const QString &description)
{
    Q_D(ModuleObject);
    if (d->m_description != description) {
        d->m_description = description;
        d->m_fallbackDescription = description;
        Q_EMIT moduleDataChanged();
    }
}
void ModuleObject::setContentText(const QStringList &contentText)
{
    Q_D(ModuleObject);
    if (d->m_contentText != contentText) {
        d->m_contentText = contentText;
        Q_EMIT moduleDataChanged();
    }
}

void ModuleObject::addContentText(const QString &contentText)
{
    Q_D(ModuleObject);
    d->m_contentText << contentText;
    Q_EMIT moduleDataChanged();
}

void ModuleObject::addContentText(const QStringList &contentText)
{
    Q_D(ModuleObject);
    d->m_contentText << contentText;
    Q_EMIT moduleDataChanged();
}

void ModuleObject::setIcon(const QVariant &icon)
{
    Q_D(ModuleObject);
    d->m_icon = icon;
    Q_EMIT moduleDataChanged();
}

void ModuleObject::setIcon(const QIcon &icon)
{
    setIcon(QVariant::fromValue(icon));
}
void ModuleObject::setBadge(int badge)
{
    Q_D(ModuleObject);
    if (d->m_badge != badge) {
        d->m_badge = badge;
        Q_EMIT moduleDataChanged();
    }
}

void ModuleObject::appendChild(ModuleObject *const module)
{
    Q_D(ModuleObject);
    if (d->m_childrens.contains(module))
        return;
    d->m_childrens.append(module);
    module->setParent(this);
    connect(module, &ModuleObject::visibleChanged, this, [this]() {
        Q_D(ModuleObject);
        d->m_fallbackDescription = d->modelDescription();
        Q_EMIT moduleDataChanged();
    });
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::removeChild(ModuleObject *const module)
{
    Q_D(ModuleObject);
    if (!d->m_childrens.contains(module))
        return;
    Q_EMIT removedChild(module);
    d->m_childrens.removeOne(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::removeChild(const int index)
{
    Q_D(ModuleObject);
    if (d->m_childrens.size() <= index)
        return;
    Q_EMIT removedChild(d->m_childrens[index]);
    d->m_childrens.removeAt(index);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module)
{
    Q_D(ModuleObject);
    if (d->m_childrens.contains(module))
        return;
    d->m_childrens.insert(before, module);
    connect(module, &ModuleObject::visibleChanged, this, [this]() {
        Q_D(ModuleObject);
        d->m_fallbackDescription = d->modelDescription();
        Q_EMIT moduleDataChanged();
    });
    module->setParent(this);
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::insertChild(const int index, ModuleObject *const module)
{
    Q_D(ModuleObject);
    if (d->m_childrens.contains(module))
        return;
    d->m_childrens.insert(index, module);
    module->setParent(this);
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

bool ModuleObject::getFlagState(uint32_t flag) const
{
    Q_D(const ModuleObject);
    return (d->m_flags & flag);
}

uint32_t ModuleObject::getFlag() const
{
    Q_D(const ModuleObject);
    return d->m_flags;
}

bool ModuleObject::extra() const
{
    return getFlagState(DCC_EXTRA);
}

void ModuleObject::setExtra(bool value)
{
    setFlagState(DCC_EXTRA, value);
}

bool ModuleObject::noSearch() const
{
    return getFlagState(DCC_NOSEARCH);
}

void ModuleObject::setNoSearch(bool noSearch)
{
    setFlagState(DCC_NOSEARCH, noSearch);
}

ModuleObject *ModuleObject::currentModule() const
{
    Q_D(const ModuleObject);
    return d->m_currentModule;
}

ModuleObject *ModuleObject::defultModule()
{
    Q_D(const ModuleObject);
    // 第一个可见项
    for (auto &&module : d->m_childrens) {
        if (!ModuleObject::IsHidden(module) && !module->extra())
            return module;
    }
    return nullptr;
}

void ModuleObject::setFlagState(uint32_t flag, bool state)
{
    Q_D(ModuleObject);
    if (getFlagState(flag) != state) {
        if (state)
            d->m_flags |= flag;
        else
            d->m_flags &= (~flag);
        Q_EMIT stateChanged(flag, state);
        ModuleObject *parent = getParent();
        if (parent)
            Q_EMIT parent->childStateChanged(this, flag, state);
    }
}

void ModuleObject::setCurrentModule(ModuleObject *child)
{
    Q_D(ModuleObject);
    if (d->m_currentModule != child) {
        d->m_currentModule = child;
        Q_EMIT currentModuleChanged(d->m_currentModule);
    }
}

ModuleObject *ModuleObject::getParent()
{
    return dynamic_cast<ModuleObject *>(parent());
}

int ModuleObject::findChild(ModuleObject *const child)
{
    if (!child)
        return -1;
    if (this == child)
        return 0;
    return findChild(this, child, 0);
}

int ModuleObject::findChild(ModuleObject *const module, ModuleObject *const child)
{
    if (!module || !child)
        return -1;
    if (module == child)
        return 0;
    return findChild(module, child, 0);
}

const QList<ModuleObject *> &ModuleObject::childrens()
{
    Q_D(const ModuleObject);
    return d->m_childrens;
}

ModuleObject *ModuleObject::children(const int index) const
{
    Q_D(const ModuleObject);
    if (index < 0 || index >= d->m_childrens.size())
        return nullptr;
    return d->m_childrens.at(index);
}

int ModuleObject::getChildrenSize() const
{
    Q_D(const ModuleObject);
    return d->m_childrens.size();
}

int ModuleObject::findChild(ModuleObject *const module, ModuleObject *const child, const int num)
{
    for (auto ch : module->childrens()) {
        if (ch == child)
            return num + 1;
    }
    for (auto ch : module->childrens()) {
        const int temp = ModuleObject::findChild(ch, child, num + 1);
        if (temp > num)
            return temp;
    }
    return -1;
}

bool ModuleObject::IsVisible(ModuleObject *const module)
{
    return !ModuleObject::IsHidden(module);
}

bool ModuleObject::IsHidden(ModuleObject *const module)
{
    return module ? module->getFlagState(DCC_ALL_HIDDEN) : true;
}

bool ModuleObject::IsHiddenFlag(uint32_t flag)
{
    return DCC_ALL_HIDDEN & flag;
}

bool ModuleObject::IsEnabled(ModuleObject *const module)
{
    return !ModuleObject::IsDisabled(module);
}

bool ModuleObject::IsDisabled(ModuleObject *const module)
{
    return module ? module->getFlagState(DCC_ALL_DISABLED) : true;
}

bool ModuleObject::IsDisabledFlag(uint32_t flag)
{
    return DCC_ALL_DISABLED & flag;
}
