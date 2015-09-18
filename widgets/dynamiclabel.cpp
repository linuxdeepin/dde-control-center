#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QResizeEvent>

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
    m_animation->setPropertyName("pos");

    connect(m_animation, &QPropertyAnimation::finished, [this]{
        update();
        if(m_label->geometry().left() == width())
            emit hideFinished();
    });
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QString DynamicLabel::text() const
{
    return m_label->text();
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

QSize DynamicLabel::sizeHint() const
{
    return m_label->sizeHint();
}

void DynamicLabel::setText(const QString &text)
{
    m_label->setText(text);
    updateGeometry();
    resize(m_label->sizeHint());
}

void DynamicLabel::showLabel()
{
    m_delayTimer.stop();
    m_animation->stop();
    m_animation->setStartValue(QPoint(width(), 0));
    m_animation->setEndValue(QPoint(width() - qMin(m_label->width(), m_label->fontMetrics().width(text())), 0));
    m_animation->start();
}

void DynamicLabel::hideLabel()
{
    m_delayTimer.stop();
    m_animation->stop();
    m_animation->setStartValue(QPoint(m_label->x(), 0));
    m_animation->setEndValue(QPoint(width(), 0));
    m_animation->start();
}

void DynamicLabel::delayShowLabel(int duration)
{
    m_delayTimer.stop();
    disconnect(&m_delayTimer, &QTimer::timeout, this, &DynamicLabel::hideLabel);
    connect(&m_delayTimer, &QTimer::timeout, this, &DynamicLabel::showLabel);
    m_delayTimer.start(duration);
}

void DynamicLabel::delayHideLabel(int duration)
{
    m_delayTimer.stop();
    disconnect(&m_delayTimer, &QTimer::timeout, this, &DynamicLabel::showLabel);
    connect(&m_delayTimer, &QTimer::timeout, this, &DynamicLabel::hideLabel);
    m_delayTimer.start(duration);
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

void DynamicLabel::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    m_label->setFixedSize(e->size());
}

