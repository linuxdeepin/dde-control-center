/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
const double VIEW_WIDTH = 400.;
const double VIEW_HEIGHT = 200.;

MonitorsGround::MonitorsGround(QWidget *parent)
    : QFrame(parent),

      m_refershTimer(new QTimer(this))
{
    m_refershTimer->setInterval(100);
    m_refershTimer->setSingleShot(true);

    connect(m_refershTimer, &QTimer::timeout, this, &MonitorsGround::resetMonitorsView);

    setFixedSize(VIEW_WIDTH + MARGIN_W * 2, VIEW_HEIGHT + MARGIN_H * 2);
#ifdef QT_DEBUG
//    setStyleSheet("background-color: cyan;");
#endif
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
        connect(pw, &MonitorProxyWidget::requestMonitorPress, this, &MonitorsGround::requestMonitorPress);
        connect(pw, &MonitorProxyWidget::requestMonitorRelease, this, &MonitorsGround::requestMonitorRelease);
        connect(mon, &Monitor::geometryChanged, m_refershTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
        connect(model, &DisplayModel::primaryScreenChanged, pw, static_cast<void (MonitorProxyWidget::*)()>(&MonitorProxyWidget::update), Qt::QueuedConnection);
    }

    QTimer::singleShot(1, this, &MonitorsGround::resetMonitorsView);
}

void MonitorsGround::resetMonitorsView()
{
    qDebug() << Q_FUNC_INFO;

    reloadViewPortSize();
    for (auto pw : m_monitors.keys())
        adjust(pw);

    Monitor* firstMonitor = m_monitors.values().first();
    for (auto it = m_monitors.cbegin(); it != m_monitors.cend(); ++it) {
        if (firstMonitor->rect() == it.value()->rect()) {
            m_model->setIsMerge(true);
            continue;
        }

        m_model->setIsMerge(false);
    }

    if (m_model->isMerge()) {
        return;
    }

    // recheck settings
    if (!isScreenPerfect()) {
        monitorMoved(m_monitors.firstKey());
    }
}

void MonitorsGround::monitorMoved(MonitorProxyWidget *pw)
{
    qDebug() << Q_FUNC_INFO << pw->name();

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
    qApp->processEvents();
    QTimer::singleShot(1, this, &MonitorsGround::resetMonitorsView);
}

void MonitorsGround::adjust(MonitorProxyWidget *pw)
{
    qDebug() << "adjust" << pw->name();

    const double scale = screenScale();

    const int offsetX = VIEW_WIDTH / 2 - (m_viewPortWidth * scale) / 2 + MARGIN_W;
    const int offsetY = VIEW_HEIGHT / 2 - (m_viewPortHeight * scale) / 2 + MARGIN_H;

    const int w = scale * pw->w();
    const int h = scale * pw->h();
    const int x = scale * pw->x();
    const int y = scale * pw->y();

    pw->setGeometry(x + offsetX, y + offsetY, w, h);
    pw->update();
}

void MonitorsGround::ensureWidgetPerfect(MonitorProxyWidget *pw)
{
    qDebug() << Q_FUNC_INFO << pw->name();

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

    const QPoint bestOffset = bestMoveOffset(pw, other);
    const int x = pw->x() - bestOffset.x();
    const int y = pw->y() - bestOffset.y();

    pw->setMovedX(x);
    pw->setMovedY(y);
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
        Q_EMIT requestApplySettings(it.value(), it.key()->x(), it.key()->y());
}

bool MonitorsGround::isScreenPerfect() const
{
    // only support 2 screens
    if (m_monitors.size() != 2)
        return true;

    MonitorProxyWidget *p0 = m_monitors.firstKey();
    MonitorProxyWidget *p1 = m_monitors.lastKey();

    const QRect r0(p0->x(), p0->y(), p0->w(), p0->h());
    const QRect r1(p1->x(), p1->y(), p1->w(), p1->h());

    if (r0 == r1)
        return true;
    if (r0.intersects(r1))
        return false;

    return bestMoveOffset(p0, p1).isNull();
}

double MonitorsGround::screenScale() const
{
    const double scaleW = VIEW_WIDTH / m_viewPortWidth;
    const double scaleH = VIEW_HEIGHT / m_viewPortHeight;

    return std::min(scaleW, scaleH);
}

const QPoint MonitorsGround::bestMoveOffset(MonitorProxyWidget *pw0, MonitorProxyWidget *pw1) const
{
    const QPoint selfTopLeft = QPoint(pw0->x(), pw0->y());
    QList<QPoint> selfPoints;
    selfPoints.append(selfTopLeft);
    selfPoints.append(QPoint(pw0->x() + pw0->w(), pw0->y()));
    selfPoints.append(QPoint(pw0->x(), pw0->y() + pw0->h()));
    selfPoints.append(QPoint(pw0->x() + pw0->w(), pw0->y() + pw0->h()));

    const QPoint otherTopLeft = QPoint(pw1->x(), pw1->y());
    const QRect r1(pw1->x(), pw1->y(), pw1->w(), pw1->h());
    QList<QPoint> otherPoints;
    otherPoints.append(otherTopLeft);
    otherPoints.append(QPoint(pw1->x() + pw1->w(), pw1->y()));
    otherPoints.append(QPoint(pw1->x(), pw1->y() + pw1->h()));
    otherPoints.append(QPoint(pw1->x() + pw1->w(), pw1->y() + pw1->h()));

    // TODO: check screen rect contains another screen and size not equal
    QPoint bestOffset;
    int min = INT_MAX;
    for (auto p1 : selfPoints)
    {
        for (auto p2 : otherPoints)
        {
            const int m = (p2 - p1).manhattanLength();
            if (m >= min)
                continue;

            const QPoint offset = p1 - p2;

            // test intersect
            const QRect r0(pw0->x() - offset.x(), pw0->y() - offset.y(), pw0->w(), pw0->h());
            if (r0.intersects(r1))
                continue;

            min = m;
            bestOffset = offset;
        }
    }

    return bestOffset;
}
