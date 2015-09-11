#include <QDebug>
#include <QTimer>
#include <QFile>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "dynamiclabel.h"

DUI_USE_NAMESPACE

DynamicLabel::DynamicLabel(QWidget *parent) :
    QFrame(parent),
    m_label(new QLabel(this)),
    m_animation(new QPropertyAnimation(this))
{
    m_label->setObjectName("Label");

    D_THEME_INIT_WIDGET(DynamicLabel);

    m_animation->setTargetObject(m_label);
    m_animation->setPropertyName("geometry");

    connect(m_animation, SIGNAL(finished()), SLOT(update()));
    connect(m_animation, &QPropertyAnimation::finished, [&]{
        if(m_label->width() == 0){
            emit hideFinished();
            hide();
        }
    });

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    hide();
}

QLabel *DynamicLabel::label() const
{
    return m_label;
}

QColor DynamicLabel::color() const
{
    return m_label->palette().color(QPalette::Foreground);
}

int DynamicLabel::duration() const
{
    return m_animation->duration();
}

QEasingCurve::Type DynamicLabel::easingType() const
{
    return m_animation->easingCurve().type();
}

void DynamicLabel::setText(const QString &text)
{
    m_label->setText(text);
    setFixedSize(m_label->sizeHint());
}

void DynamicLabel::showLabel()
{
    m_animation->stop();

    show();
    QRect rect = this->rect();
    rect.setX(width());
    m_animation->setStartValue(rect);
    rect.setX(0);
    m_animation->setEndValue(rect);
    m_animation->start();

    update();
}

void DynamicLabel::hideLabel()
{
    if(isHidden())
        return;

    QRect rect = this->rect();
    rect.setX(0);
    m_animation->setStartValue(rect);
    rect.setX(width());
    m_animation->setEndValue(rect);
    m_animation->start();
}

void DynamicLabel::setColor(QColor color)
{
    QPalette pa = m_label->palette();
    pa.setColor(QPalette::Foreground, color);
}

void DynamicLabel::setDuration(int duration)
{
    m_animation->setDuration(duration);
}

void DynamicLabel::setEasingType(QEasingCurve::Type easingType)
{
    m_animation->setEasingCurve(easingType);
}

