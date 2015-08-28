#include <QHBoxLayout>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

#include "displaymodeitem.h"

DUI_USE_NAMESPACE

DisplayModeItem::DisplayModeItem(bool active, QLabel *parent) :
    QLabel(parent),
    m_title(new QLabel),
    m_text(new QLabel),
    m_active(active)
{
    D_THEME_INIT_WIDGET(DisplayModeItem);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_title);
    layout->addWidget(m_text);
    if(!active)
        layout->addWidget(new DSeparatorHorizontal);
    layout->setMargin(0);
    setLayout(layout);
}

QString DisplayModeItem::iconName() const
{
    return m_iconName;
}

QString DisplayModeItem::title() const
{
    return m_title->text();
}

QString DisplayModeItem::text() const
{
    return m_text->text();
}

bool DisplayModeItem::active() const
{
    return m_active;
}

void DisplayModeItem::setIconName(QString iconName)
{
    if(m_iconName == iconName)
        return;

    m_iconName = iconName;

    D_THEME_INIT_WIDGET(DisplayModeItem);
}

void DisplayModeItem::setTitle(QString title)
{
    m_title->setText(title);
}

void DisplayModeItem::setText(QString text)
{
    m_text->setText(text);
}

void DisplayModeItem::setActive(bool active)
{
    if(m_active == active)
        return;

    m_active = active;

    D_THEME_INIT_WIDGET(DisplayModeItem);
}
