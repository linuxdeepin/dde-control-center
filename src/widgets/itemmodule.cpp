#include "widgets/itemmodule.h"

#include <QMap>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <DLabel>
#include <DFontSizeManager>

#include <widgets/settingsitem.h>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_BEGIN_NAMESPACE
class ItemModulePrivate
{
public:
    explicit ItemModulePrivate(ItemModule *object)
        : q_ptr(object)
        , m_callback(nullptr)
        , m_background(false)
        , m_wordWrap(true)
        , m_isTitle(false)
        , m_leftVisible(true)
    {
    }
    void setCallback(std::function<QWidget *()> callback)
    {
        m_callback = callback;
    }

public:
    ItemModule *q_ptr;
    std::function<QWidget *()> m_callback;
    bool m_background;
    bool m_wordWrap;
    bool m_isTitle;
    bool m_leftVisible;
    Q_DECLARE_PUBLIC(ItemModule)
};
DCC_END_NAMESPACE

ItemModule::ItemModule(QObject *parent)
    : ModuleObject(parent)
    , DCC_INIT_PRIVATE(ItemModule)
{
}

ItemModule::ItemModule(const QString &name, const QString &displayName, bool isTitle)
    : ItemModule(nullptr)
{
    setName(name);
    setDisplayName(displayName);
    setTitleItem(isTitle);
}

ItemModule::~ItemModule()
{
}

void ItemModule::setTitleItem(bool isTitle)
{
    Q_D(ItemModule);
    d->m_isTitle = isTitle;
}

void ItemModule::setBackground(bool has)
{
    Q_D(ItemModule);
    d->m_background = has;
}

void ItemModule::setWordWrap(bool on)
{
    Q_D(ItemModule);
    d->m_wordWrap = on;
}

bool ItemModule::wordWrap() const
{
    Q_D(const ItemModule);
    return d->m_wordWrap;
}

void ItemModule::setLeftVisible(bool visible)
{
    Q_D(ItemModule);
    d->m_leftVisible = visible;
}

QWidget *ItemModule::page()
{
    Q_D(ItemModule);
    QWidget *ret = nullptr;
    if (d->m_callback)
        ret = d->m_callback();

    if (d->m_leftVisible) {
        SettingsItem *w = new SettingsItem();
        w->setAccessibleName(name());
        if (d->m_background)
            w->addBackground();
        QHBoxLayout *layout = new QHBoxLayout(w);
        DLabel *leftWidget = new DLabel(displayName());
        leftWidget->setAccessibleName(name());
        leftWidget->setWordWrap(d->m_wordWrap);
        if (d->m_isTitle) {
            leftWidget->setForegroundRole(DPalette::TextTitle);
            DFontSizeManager::instance()->bind(leftWidget, DFontSizeManager::T5, QFont::DemiBold);
            layout->addWidget(leftWidget, 0, Qt::AlignVCenter);
            if (ret)
                layout->addWidget(ret, 0, Qt::AlignVCenter | Qt::AlignRight);
        } else {
            layout->addWidget(leftWidget, 3, Qt::AlignVCenter);
            if (ret)
                layout->addWidget(ret, 7, Qt::AlignVCenter);
        }
        ret = w;
    }
    return ret;
}

void ItemModule::setCallback(std::function<QWidget *()> callback)
{
    Q_D(ItemModule);
    d->setCallback(callback);
}