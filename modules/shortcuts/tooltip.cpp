#include <QDebug>
#include <QVariant>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "tooltip.h"

DUI_USE_NAMESPACE

ToolTip::ToolTip(QWidget *parent) :
    QLabel(parent),
    m_timeoutDestroy(false),
    m_timeout(-1),
    m_timer(0),
    m_animation(new QPropertyAnimation(this))
{
    D_THEME_INIT_WIDGET(ToolTip);

    m_widget = this;

    setWordWrap(true);

    m_animation->setTargetObject(this);
    m_animation->setPropertyName("maximumHeight");

    connect(m_animation, &QPropertyAnimation::finished, [&]{
        if(height()>0&&m_timer){
            m_timer->start(m_timeout);
        }else if(m_timeoutDestroy&&m_timer&&m_timer->remainingTime()<=0){
            deleteLater();
        }
        if(height()==0)
            emit contracted();
    });
}

QStringList ToolTip::keyWords() const
{
    return QStringList();
}

void ToolTip::setData(const QVariant &datas)
{
    if(datas.type() == QVariant::String){
        setText(datas.toString());
    }
}

QVariant ToolTip::getData()
{
    return text();
}

QWidget *ToolTip::widget() const
{
    return m_widget;
}

bool ToolTip::timeoutDestroy() const
{
    return m_timeoutDestroy;
}

int ToolTip::timeout() const
{
    return m_timeout;
}

void ToolTip::setTimeoutDestroy(bool timeoutDestroy)
{
    m_timeoutDestroy = timeoutDestroy;
}

void ToolTip::setTimeout(int timeout)
{
    m_timeout = timeout;

    if(timeout>0){
        if(!m_timer)
            m_timer = new QTimer(this);
        m_timer->setSingleShot(true);
        connect(m_timer, &QTimer::timeout, this, &ToolTip::contraction);
    }else{
        if(m_timer){
            disconnect(connect(m_timer, &QTimer::timeout, this, &ToolTip::contraction));
            m_timer->deleteLater();
        }
    }
}

void ToolTip::expansion()
{
    setFixedHeight(0);
    m_animation->setStartValue(0);
    m_animation->setEndValue(sizeHint().height());
    m_animation->start();
    emit expanded();
}

void ToolTip::contraction()
{
    m_animation->setStartValue(sizeHint().height());
    m_animation->setEndValue(0);
    m_animation->start();
}

