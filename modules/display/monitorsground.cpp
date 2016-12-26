#include "monitorsground.h"
#include "monitorproxywidget.h"
#include "displaymodel.h"
#include "monitorproxywidget.h"
#include "monitor.h"

#include <QPainter>
#include <QDebug>

using namespace dcc::display;

const int MARGIN_W = 20;
const int MARGIN_H = 10;
const double VIEW_WIDTH = 300.;
const double VIEW_HEIGHT = 150.;

MonitorsGround::MonitorsGround(QWidget *parent)
    : QFrame(parent),

      m_refershTimer(new QTimer(this))
{
    m_refershTimer->setInterval(100);
    m_refershTimer->setSingleShot(true);

    connect(m_refershTimer, &QTimer::timeout, this, &MonitorsGround::resetMonitorsView);

    setFixedSize(VIEW_WIDTH + MARGIN_W * 2, VIEW_HEIGHT + MARGIN_H * 2);
    setStyleSheet("background-color: cyan;");
}

MonitorsGround::~MonitorsGround()
{
    qDeleteAll(m_monitors.keys());
}

void MonitorsGround::setDisplayModel(DisplayModel *model)
{
    m_model = model;
    m_viewPortWidth = model->screenWidth();
    m_viewPortHeight = model->screenHeight();

    for (auto mon : model->monitorList())
    {
        MonitorProxyWidget *pw = new MonitorProxyWidget(mon, this);
        m_monitors[pw] = mon;

        connect(pw, &MonitorProxyWidget::requestApplyMove, this, &MonitorsGround::monitorMoved);
        connect(mon, &Monitor::geometryChanged, m_refershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    }

    resetMonitorsView();
}

void MonitorsGround::resetMonitorsView()
{
    reloadViewPortSize();
    for (auto pw : m_monitors.keys())
        adjust(pw);
}

void MonitorsGround::monitorMoved(MonitorProxyWidget *pw)
{
    const double scale = screenScale();
    const int offsetX = VIEW_WIDTH / 2 - (m_viewPortWidth * scale) / 2 + MARGIN_W;
    const int offsetY = VIEW_HEIGHT / 2 - (m_viewPortHeight * scale) / 2 + MARGIN_H;

    pw->setMovedX((pw->pos().x() - offsetX) / scale);
    pw->setMovedY((pw->pos().y() - offsetY) / scale);

    // ensure screens is 贴合但不相交
    ensureWidgetPerfect(pw);

    // clear global offset
    int minX = INT_MAX;
    int minY = INT_MAX;
    for (auto w : m_monitors.keys())
    {
        minX = std::min(minX, w->x());
        minY = std::min(minY, w->y());
    }
    for (auto w : m_monitors.keys())
    {
        w->setMovedX(w->x() - minX);
        w->setMovedY(w->y() - minY);
    }

    applySettings();
    resetMonitorsView();
}

void MonitorsGround::adjust(MonitorProxyWidget *pw)
{
    const double scale = screenScale();

    const int offsetX = VIEW_WIDTH / 2 - (m_viewPortWidth * scale) / 2 + MARGIN_W;
    const int offsetY = VIEW_HEIGHT / 2 - (m_viewPortHeight * scale) / 2 + MARGIN_H;

    const int w = scale * pw->w();
    const int h = scale * pw->h();
    const int x = scale * pw->x();
    const int y = scale * pw->y();

    pw->setGeometry(x + offsetX, y + offsetY, w, h);
}

void MonitorsGround::ensureWidgetPerfect(MonitorProxyWidget *pw)
{
    // TODO: only support 2 screens
    if (m_monitors.size() != 2)
        return;

    MonitorProxyWidget *other = nullptr;
    for (auto w : m_monitors.keys())
    {
        if (w != pw)
        {
            other = w;
            break;
        }
    }
    Q_ASSERT(other);

    QList<QPoint> selfPoints;
    selfPoints.append(QPoint(pw->x(), pw->y()));
    selfPoints.append(QPoint(pw->x() + pw->w(), pw->y()));
    selfPoints.append(QPoint(pw->x(), pw->y() + pw->h() + 1));
    selfPoints.append(QPoint(pw->x() + pw->w(), pw->y() + pw->h() + 1));

    QList<QPoint> otherPoints;
    otherPoints.append(QPoint(other->x(), other->y()));
    otherPoints.append(QPoint(other->x() + other->w(), other->y()));
    otherPoints.append(QPoint(other->x(), other->y() + other->h() + 1));
    otherPoints.append(QPoint(other->x() + other->w(), other->y() + other->h() + 1));

    QPoint bestOffset;
    int min = INT_MAX;
    for (auto p1 : selfPoints)
    {
        for (auto p2 : otherPoints)
        {
            const int m = (p2 - p1).manhattanLength();
            if (m < min)
            {
                min = m;
                bestOffset = p1 - p2;
            }
        }
    }

    pw->setMovedX(pw->x() - bestOffset.x());
    pw->setMovedY(pw->y() - bestOffset.y());
}

void MonitorsGround::reloadViewPortSize()
{
    int w = 0;
    for (auto pw : m_monitors.keys())
        w = std::max(w, pw->x() + pw->w());

    int h = 0;
    for (auto pw : m_monitors.keys())
        h = std::max(h, pw->y() + pw->h());

    m_viewPortWidth = w;
    m_viewPortHeight = h;
}

void MonitorsGround::applySettings()
{
    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it)
        emit requestApplySettings(it.value(), it.key()->x(), it.key()->y());
}

double MonitorsGround::screenScale() const
{
    const double scaleW = VIEW_WIDTH / m_viewPortWidth;
    const double scaleH = VIEW_HEIGHT / m_viewPortHeight;

    return std::min(scaleW, scaleH);
}
