#include "widgets/settingsgroupmodule.h"

#include <QMap>
#include <QWidget>
#include <QHBoxLayout>

DCC_USE_NAMESPACE

DCC_BEGIN_NAMESPACE
class SettingsGroupModulePrivate
{
public:
    explicit SettingsGroupModulePrivate(SettingsGroupModule *object)
        : q_ptr(object)
        , bgStyle(SettingsGroup::GroupBackground)
        , hor(QSizePolicy::Expanding)
        , ver(QSizePolicy::Fixed)
        , spacing(1)
        , headerVisible(false)
    {
    }

    QWidget *page()
    {
        Q_Q(SettingsGroupModule);
        settingsGroup = new SettingsGroup(nullptr, bgStyle);
        m_mapWidget.clear();
        QObject::connect(settingsGroup, &QObject::destroyed, q, [this]() { m_mapWidget.clear(); });
        settingsGroup->setHeaderVisible(headerVisible);
        settingsGroup->setSpacing(spacing);

        settingsGroup->getLayout()->setContentsMargins(0, 0, 0, 0);
        settingsGroup->layout()->setMargin(0);
        settingsGroup->setSizePolicy(hor, ver);

        for (auto &&tmpChild : q->childrens()) {
            QWidget *childPage = tmpChild->activePage();
            if (childPage) {
                settingsGroup->insertWidget(childPage);
                m_mapWidget.insert(tmpChild, childPage);
            }
        }
        // 监听子项的添加、删除、状态变更，动态的更新界面
        auto addModuleSlot = [this](ModuleObject *const tmpChild) {
            onAddChild(tmpChild);
        };
        QObject::connect(q, &ModuleObject::insertedChild, settingsGroup, addModuleSlot);
        QObject::connect(q, &ModuleObject::appendedChild, settingsGroup, addModuleSlot);
        QObject::connect(q, &ModuleObject::removedChild, settingsGroup, [this](ModuleObject *const childModule) { onRemoveChild(childModule); });
        QObject::connect(q, &ModuleObject::childStateChanged, settingsGroup, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            if (ModuleObject::IsHidenFlag(flag)) {
                if (state)
                    onRemoveChild(tmpChild);
                else
                    onAddChild(tmpChild);
            }
        });
        return settingsGroup;
    }

private:
    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (m_mapWidget.contains(childModule)) {
            QWidget *w = m_mapWidget.value(childModule);
            QVBoxLayout *layout = settingsGroup->getLayout();
            int index = layout->indexOf(w);
            if (-1 != index) {
                w->deleteLater();
                delete layout->takeAt(index);
                m_mapWidget.remove(childModule);
                return;
            }
        }
    }
    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule)
    {
        if (ModuleObject::IsHiden(childModule) || m_mapWidget.contains(childModule))
            return;

        Q_Q(SettingsGroupModule);
        bool isExtra = childModule->extra();
        int index = 0;
        for (auto &&child : q->childrens()) {
            if (child == childModule)
                break;
            if (!ModuleObject::IsHiden(child) && child->extra() == isExtra)
                index++;
        }
        auto newPage = childModule->activePage();
        if (newPage) {
            QVBoxLayout *layout = settingsGroup->getLayout();
            layout->insertWidget(index, newPage);
            m_mapWidget.insert(childModule, newPage);
        }
    }

public:
    SettingsGroupModule *q_ptr;
    Q_DECLARE_PUBLIC(SettingsGroupModule)
    SettingsGroup::BackgroundStyle bgStyle;
    QSizePolicy::Policy hor;
    QSizePolicy::Policy ver;
    int spacing;
    bool headerVisible;
    SettingsGroup *settingsGroup;
    QMap<ModuleObject *, QWidget *> m_mapWidget;
};
DCC_END_NAMESPACE

SettingsGroupModule::SettingsGroupModule(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, parent)
    , DCC_INIT_PRIVATE(SettingsGroupModule)
{
}

SettingsGroupModule::~SettingsGroupModule()
{
}

void SettingsGroupModule::setHeaderVisible(const bool visible)
{
    Q_D(SettingsGroupModule);
    d->headerVisible = visible;
}

void SettingsGroupModule::setSpacing(const int spacing)
{
    Q_D(SettingsGroupModule);
    d->spacing = spacing;
}

void SettingsGroupModule::setBackgroundStyle(SettingsGroup::BackgroundStyle bgStyle)
{
    Q_D(SettingsGroupModule);
    d->bgStyle = bgStyle;
}

SettingsGroup::BackgroundStyle SettingsGroupModule::backgroundStyle() const
{
    Q_D(const SettingsGroupModule);
    return d->bgStyle;
}

void SettingsGroupModule::setSizePolicy(QSizePolicy::Policy hor, QSizePolicy::Policy ver)
{
    Q_D(SettingsGroupModule);
    d->hor = hor;
    d->ver = ver;
}

QWidget *SettingsGroupModule::page()
{
    Q_D(SettingsGroupModule);
    return d->page();
}
