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
        , spacing(0)
        , headerVisible(false)
    {
    }

public:
    SettingsGroupModule *q_ptr;
    Q_DECLARE_PUBLIC(SettingsGroupModule)
    SettingsGroup::BackgroundStyle bgStyle;
    QSizePolicy::Policy hor;
    QSizePolicy::Policy ver;
    int spacing;
    bool headerVisible;
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
    SettingsGroup *settingsGroup = new SettingsGroup(nullptr, d->bgStyle);

    settingsGroup->setHeaderVisible(d->headerVisible);
    settingsGroup->setSpacing(d->spacing);

    settingsGroup->getLayout()->setContentsMargins(0, 0, 0, 0);
    settingsGroup->layout()->setMargin(0);
    settingsGroup->setSizePolicy(d->hor, d->ver);

    for (auto &&tmpChild : childrens()) {
        QWidget *childPage = tmpChild->activePage();
        if (childPage) {
            settingsGroup->insertWidget(childPage);
            childPage->setEnabled(!ModuleObject::IsDisabled(tmpChild));
            childPage->setVisible(!ModuleObject::IsHiden(tmpChild));
            connect(tmpChild, &ModuleObject::stateChanged, childPage, [childPage](uint32_t flag, bool state) {
                if (ModuleObject::IsHidenFlag(flag))
                    childPage->setVisible(!state);
                else if (ModuleObject::IsDisabledFlag(flag))
                    childPage->setEnabled(!state);
            });
        }
    }
    return settingsGroup;
}
