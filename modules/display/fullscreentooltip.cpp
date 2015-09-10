#include <QPainter>
#include <QDebug>

#include "fullscreentooltip.h"
#include "dbus/monitorinterface.h"

FullScreenTooltip::FullScreenTooltip(MonitorInterface *dbus) :
    QFrame(nullptr),
    m_dbusMonitor(dbus)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool | Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);

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
        int fontSize = m_monitorRect.height() / 5;
        QFont font;
        font.setPixelSize(fontSize);
        setFont(font);

        rect = QRect(0, 0, fontMetrics().width(name), fontMetrics().height());
        rect.moveCenter(m_monitorRect.center());
        setGeometry(rect);

        QPainterPathStroker pps;
        pps.setCapStyle(Qt::RoundCap);
        pps.setWidth(1);

        pa.setPen(Qt::white);
        pa.setOpacity(0.8);
        pa.drawText(this->rect(), name, option);

        QPainterPath path;
        path.addText(0, rect.height() / 1.145, font, name);
        path = pps.createStroke(path).united(path);
        pa.strokePath(path, QPen(Qt::black));
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
        font.setPixelSize(rect.height() / 1.5);
        setFont(font);
        pa.drawText(this->rect(), name, option);
    }

    pa.end();
}

