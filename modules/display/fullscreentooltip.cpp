#include <QPainter>
#include <QDebug>
#include <QX11Info>

#include "fullscreentooltip.h"
#include "dbus/monitorinterface.h"

#include <X11/extensions/shape.h>

FullScreenTooltip::FullScreenTooltip(MonitorInterface *dbus) :
    QFrame(nullptr),
    m_dbusMonitor(dbus)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // pass mouse event
    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput, 0, 0, nullptr, 0, ShapeSet, YXBanded);

    connect(m_dbusMonitor, &MonitorInterface::XChanged, [this]{
        m_monitorRect.moveLeft(m_dbusMonitor->x());
        update();
    });
    connect(m_dbusMonitor, &MonitorInterface::YChanged, [this]{
        m_monitorRect.moveTop(m_dbusMonitor->y());
        update();
    });
    connect(m_dbusMonitor, &MonitorInterface::WidthChanged, [this]{
        m_monitorRect.setWidth(m_dbusMonitor->width());
        update();
    });
    connect(m_dbusMonitor, &MonitorInterface::HeightChanged, [this]{
        m_monitorRect.setHeight(m_dbusMonitor->height());
        update();
    });

    m_monitorRect.moveLeft(m_dbusMonitor->x());
    m_monitorRect.moveTop(m_dbusMonitor->y());
    m_monitorRect.setWidth(m_dbusMonitor->width());
    m_monitorRect.setHeight(m_dbusMonitor->height());
}

void FullScreenTooltip::showToCenter()
{
    m_showCenter = true;

    show();
    update();
}

void FullScreenTooltip::showToTopLeft()
{
    m_showCenter = false;
    setGeometry(QRect(m_monitorRect.left(), m_monitorRect.top(), 100, 50));
    show();
    update();
}

void FullScreenTooltip::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter pa(this);

    QTextOption option;
    option.setFlags(QTextOption::ShowLineAndParagraphSeparators);
    option.setAlignment(Qt::AlignCenter);
    QRect rect;

    QString name  = m_dbusMonitor->name();

    if(m_showCenter){
        move(m_monitorRect.topLeft());
        setFixedSize(m_monitorRect.size());

        int fontSize = m_monitorRect.height() / 5;
        QFont font;
        font.setPixelSize(fontSize);
        setFont(font);

        QPen pen;
        pen.setWidth(1);
        pen.setColor(Qt::black);

        QBrush brush;
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);

        const int x = m_monitorRect.center().x() - m_monitorRect.left() - fontMetrics().width(name) / 2;
        const int y = m_monitorRect.center().y() + fontMetrics().height() / 2;

        QPainterPath path;
        path.addText(x, y, font, name);

        pa.setRenderHint(QPainter::Antialiasing, true);
        pa.setFont(font);
        pa.setPen(pen);
        pa.setBrush(brush);
        pa.drawPath(path);
    }else{
        rect = QRect(m_monitorRect.left(), m_monitorRect.top(), 100, 50);
        setGeometry(rect);
        pa.fillRect(this->rect(), Qt::yellow);
        QPen pen;
        pen.setWidth(5);
        pen.setColor(Qt::black);
        pa.setPen(pen);
        pa.drawRect(this->rect());
        QFont font;
        font.setPixelSize(qMin<int>(rect.width() / (name.size() - 2), rect.height() / 1.5));
        setFont(font);
        pa.drawText(this->rect(), name, option);
    }

    pa.end();
}

