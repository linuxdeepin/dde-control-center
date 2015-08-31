#include <QHBoxLayout>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>
#include <libdui/dseparatorhorizontal.h>

#include "displaymodeitem.h"

DUI_USE_NAMESPACE

DisplayModeItem::DisplayModeItem(bool showSeparator, QLabel *parent) :
    QLabel(parent),
    m_title(new QLabel),
    m_text(new QLabel),
    m_active(false),
    m_hover(false)
{
    m_title->setObjectName("Title");
    m_text->setObjectName("Text");
    m_text->setWordWrap(true);

    D_THEME_INIT_WIDGET(DisplayModeItem, hover, active);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_title);
    layout->addWidget(m_text);
    if(showSeparator){
        layout->addSpacing(10);
        layout->addWidget(new DSeparatorHorizontal);
    }
    layout->setMargin(0);
    layout->setContentsMargins(60, 0, 0, 0);
    setLayout(layout);
}

DisplayModeItem::~DisplayModeItem()
{
    if(m_active)
        activeItem = NULL;
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

bool DisplayModeItem::hover() const
{
    return m_hover;
}

void DisplayModeItem::setIconName(QString iconName)
{
    if(m_iconName == iconName)
        return;

    m_iconName = iconName;

    updateIcon();
}

void DisplayModeItem::setTitle(QString title)
{
    m_title->setText(title);
}

void DisplayModeItem::setText(QString text)
{
    m_text->setText(text);
}

DisplayModeItem *DisplayModeItem::activeItem = NULL;

void DisplayModeItem::setActive(bool active)
{
    if(m_active == active)
        return;

    m_active = active;

    updateIcon();

    if(active){
        if(activeItem)
            activeItem->setActive(false);
        activeItem = this;
    }else{
        activeItem = NULL;
    }

    emit activeChanged(active);
}

void DisplayModeItem::enterEvent(QEvent *e)
{
    QLabel::enterEvent(e);

    setHover(true);
}

void DisplayModeItem::leaveEvent(QEvent *e)
{
    QLabel::leaveEvent(e);

    setHover(false);
}

void DisplayModeItem::mouseReleaseEvent(QMouseEvent *e)
{
    QLabel::mouseReleaseEvent(e);

    setActive(true);
}

void DisplayModeItem::setHover(bool arg)
{
    if(m_hover == arg)
        return;

    m_hover = arg;

    updateIcon();

    emit hoverChanged(arg);
}

void DisplayModeItem::updateIcon()
{
    if(m_active){
        setPixmap(QPixmap(QString(":/images/%1_active.png").arg(m_iconName)));
    }else{
        if(m_hover){
            setPixmap(QPixmap(QString(":/images/%1_hover.png").arg(m_iconName)));
        }else{
            setPixmap(QPixmap(QString(":/images/%1_normal.png").arg(m_iconName)));
        }
    }
}
