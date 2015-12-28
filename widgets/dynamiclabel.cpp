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
    m_showAnimation(new QPropertyAnimation(this)),
    m_hideAnimation(new QPropertyAnimation(this))
{
    m_label->setObjectName("Label");

    D_THEME_INIT_WIDGET(DynamicLabel);

    m_showAnimation->setTargetObject(m_label);
    m_showAnimation->setPropertyName("pos");
    m_hideAnimation->setTargetObject(m_label);
    m_hideAnimation->setPropertyName("pos");

    connect(m_hideAnimation, &QPropertyAnimation::finished, [this]{
        update();
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
    return showDuration();
}

QEasingCurve::Type DynamicLabel::easingType() const
{
    return showEasingType();
}

QSize DynamicLabel::sizeHint() const
{
    return m_label->sizeHint();
}

int DynamicLabel::hideDuration() const
{
    return m_hideAnimation->duration();
}

QEasingCurve::Type DynamicLabel::hideEasingType() const
{
    return m_hideAnimation->easingCurve().type();
}

int DynamicLabel::showDuration() const
{
    return m_showAnimation->duration();
}

QEasingCurve::Type DynamicLabel::showEasingType() const
{
    return m_showAnimation->easingCurve().type();
}

void DynamicLabel::setText(const QString &text)
{
    m_label->setText(text);
    updateGeometry();
    resize(m_label->sizeHint());
}

void DynamicLabel::showLabel()
{
    labelIsVisible = true;

    m_showAnimation->stop();
    m_delayTimer.stop();
    m_showAnimation->setStartValue(QPoint(width(), 0));
    m_showAnimation->setEndValue(QPoint(width() - qMin(m_label->width(), m_label->fontMetrics().width(text())), 0));
    m_showAnimation->start();
}

void DynamicLabel::hideLabel()
{
    labelIsVisible = false;

    m_delayTimer.stop();

    m_hideAnimation->setStartValue(QPoint(m_label->x(), 0));
    m_hideAnimation->setEndValue(QPoint(width(), 0));
    m_hideAnimation->start();
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
    setShowDuration(duration);
}

void DynamicLabel::setEasingType(QEasingCurve::Type easingType)
{
    setShowEasingType(easingType);
}

void DynamicLabel::setHideDuration(int hideDuration)
{
    m_hideAnimation->setDuration(hideDuration);
}

void DynamicLabel::setHideEasingTyp(QEasingCurve::Type hideEasingType)
{
    m_hideAnimation->setEasingCurve(hideEasingType);
}

void DynamicLabel::setShowDuration(int showDuration)
{
    m_showAnimation->setDuration(showDuration);
}

void DynamicLabel::setShowEasingType(QEasingCurve::Type showEasingType)
{
    m_showAnimation->setEasingCurve(showEasingType);
}

void DynamicLabel::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);
    m_label->setFixedSize(e->size());

    if(labelIsVisible) {
        int end_x = e->size().width() - qMin(m_label->width(),
                     m_label->fontMetrics().width(text()));

        if(m_showAnimation->state() == QVariantAnimation::Running) {
            m_showAnimation->setEndValue(QPoint(end_x, 0));
        } else {
            m_label->move(end_x, 0);
        }
    } else {
        if(m_hideAnimation->state() == QVariantAnimation::Running) {
            m_hideAnimation->setEndValue(QPoint(e->size().width(), 0));
        } else {
            m_label->move(e->size().width(), 0);
        }
    }
}

