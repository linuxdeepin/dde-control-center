#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QDebug>
#include <QTimer>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "monitor.h"

DUI_USE_NAMESPACE

Monitor::Monitor(QWidget *parent) :
    QFrame(parent),
    m_draggable(false),
    m_nameAlignment(Qt::AlignCenter),
    m_brother(NULL),
    m_draging(false),
    m_eyeing(false),
    m_primary(false),
    m_child(NULL)
{
    D_THEME_INIT_WIDGET(Monitor);

    setFocusPolicy(Qt::ClickFocus);
}


void Monitor::setName(QString name)
{
    if(m_name == name)
        return;

    m_name = name;

    update();
}

QSize Monitor::resolution()
{
    return m_resolution;
}

void Monitor::setResolution(int width, int height)
{
    m_resolution = QSize(width, height);
}

QColor Monitor::dockBgColor() const
{
    return m_dockBgColor;
}

bool Monitor::draggable() const
{
    return m_draggable;
}

QString Monitor::name() const
{
    return m_name;
}

Qt::Alignment Monitor::nameAlignment() const
{
    return m_nameAlignment;
}

Monitor *Monitor::brother() const
{
    return m_brother;
}

bool Monitor::draging() const
{
    return m_draging;
}

bool Monitor::eyeing() const
{
    return m_eyeing;
}

bool Monitor::primary() const
{
    return m_primary;
}

const Monitor *Monitor::child() const
{
    return m_child;
}

void Monitor::setDockBgColor(QColor dockBgColor)
{
    if(dockBgColor == m_dockBgColor)
        return;

    m_dockBgColor = dockBgColor;

    update();
}

void Monitor::setDraggable(bool draggable)
{
    m_draggable = draggable;
}

void Monitor::setAlignment(Qt::Alignment aalignment)
{
    m_nameAlignment = aalignment;
}

void Monitor::setNameAlignment(Qt::Alignment nameAlignment)
{
    m_nameAlignment = nameAlignment;
}

void Monitor::setBrother(Monitor *brother)
{
    m_brother = brother;
}

void Monitor::setPrimary(bool primary)
{
    if(m_primary == primary)
        return;

    m_primary = primary;

    update();
}

void Monitor::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);

    QPainter pa;
    pa.begin(this);

    QRect child_rect;

    if(m_child){
        child_rect.setWidth(width() * 0.5625);
        child_rect.setHeight(height() * 0.5);
        child_rect.moveTo(width() / 8, height() / 6);

        QRect me_rect = child_rect;
        me_rect.moveTopLeft(QPoint(child_rect.right() - child_rect.width() * 0.667,
                                   child_rect.bottom() - child_rect.height() * 0.667));

        QTextOption tmpOption;
        QFont font = pa.font();

        tmpOption.setAlignment(Qt::AlignCenter);
        font.setPixelSize(child_rect.height() / 2);

        pa.fillRect(child_rect, Qt::black);
        pa.setFont(font);
        pa.drawText(child_rect, "A", tmpOption);
        QPen tmp_pen = pa.pen();
        pa.setPen(QColor("#4b4b4b"));
        pa.drawRect(child_rect);

        pa.setOpacity(0.95);
        pa.fillRect(me_rect, Qt::black);
        pa.setOpacity(1);
        pa.drawRect(me_rect);
        pa.setPen(tmp_pen);
        pa.drawText(me_rect, "A", tmpOption);

        font.setPixelSize(12);
        pa.setFont(font);
        tmpOption.setAlignment(Qt::AlignRight|Qt::AlignTop);

        pa.drawText(child_rect, m_child->name(), tmpOption);
        pa.drawText(me_rect, m_name, tmpOption);
    }else{
        int top_width = width() / 3.0;
        int height = top_width / 8.0;

        QTextOption nameOption;
        nameOption.setAlignment(m_nameAlignment);
        pa.drawText(rect(), m_name, nameOption);

        if(m_draging && m_brother){
            pa.drawText(10, 20, QString("(%1,%2)").arg(x() - m_brother->x()).arg(y() - m_brother->y()));
        }

        if(m_primary){
            QPainterPath path;
            path.moveTo(top_width, this->height() - height);
            path.lineTo(top_width * 2, this->height() - height);
            path.lineTo(top_width * 2.1, this->height());
            path.lineTo(top_width * 0.9, this->height());
            path.lineTo(top_width, this->height() - height);

            pa.setRenderHint(QPainter::Antialiasing);
            pa.fillPath(path, m_dockBgColor);
        }
    }

    pa.end();
}

void Monitor::mousePressEvent(QMouseEvent *e)
{
    QFrame::mousePressEvent(e);

    if(!m_draggable){
        e->ignore();
        return;
    }

    m_oldPos = e->pos();

    setCursor(Qt::ClosedHandCursor);

    setDraging(true);

    emit mousePressed(e->pos());
}

void Monitor::mouseMoveEvent(QMouseEvent *e)
{
    QFrame::mouseMoveEvent(e);

    if(!m_draging){
        e->ignore();
        return;
    }

    move(pos() + e->pos() - m_oldPos);

    emit mouseMoveing(e->pos());
}

void Monitor::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    if(!m_draging){
        e->ignore();
        return;
    }

    setCursor(Qt::ArrowCursor);

    setDraging(false);

    emit mouseRelease(e->pos());
}

bool Monitor::dragEnter(Monitor *monitor)
{
    if(m_child || monitor == this)
        return false;

    setEyeing(true);

    return true;
}

void Monitor::dragLeave()
{
    setEyeing(false);
}

bool Monitor::drop(Monitor *monitor)
{
    if(m_child || monitor == this)
        return false;

    setEyeing(false);

    m_child = monitor;
    m_child->hide();

    update();

    return true;
}

Monitor *Monitor::split()
{
    m_child = NULL;

    update();

    return m_child;
}

void Monitor::setDraging(bool arg)
{
    if(m_draging == arg)
        return;

    m_draging = arg;

    D_THEME_INIT_WIDGET(Monitor);

    emit dragingChanged(arg);
}

void Monitor::setEyeing(bool arg)
{
    if(m_eyeing == arg)
        return;

    m_eyeing = arg;

    D_THEME_INIT_WIDGET(Monitor);
}
