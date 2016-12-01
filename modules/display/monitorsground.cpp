#include "monitorsground.h"
#include "monitorproxywidget.h"
#include "displaymodel.h"
#include "monitorproxywidget.h"
#include "monitor.h"

#include <QPainter>
#include <QDebug>

const int MARGIN_W = 50;
const int MARGIN_H = 25;
const double VIEW_WIDTH = 300.;
const double VIEW_HEIGHT = 150.;

MonitorsGround::MonitorsGround(QWidget *parent)
    : QFrame(parent)
{
    setFixedSize(VIEW_WIDTH + MARGIN_W * 2, VIEW_HEIGHT + MARGIN_H * 2);
    setStyleSheet("background-color: cyan;");
}

MonitorsGround::~MonitorsGround()
{
    // TODO: delete monitor proxy widgets
}

void MonitorsGround::setDisplayModel(DisplayModel *model)
{
    m_model = model;

    for (auto mon : model->monitorList())
    {
        MonitorProxyWidget *pw = new MonitorProxyWidget(mon, this);
        m_monitors[mon] = pw;

        connect(pw, &MonitorProxyWidget::requestApplyMove, this, &MonitorsGround::monitorMoved);

        adjust(pw);
    }
}

void MonitorsGround::monitorMoved(MonitorProxyWidget *pw)
{
    if (pw->name() != m_model->primary())
    {
        const double scale = screenScale();
        const int offsetX = VIEW_WIDTH / 2 - (m_model->screenWidth() * scale) / 2 + MARGIN_W;
        const int offsetY = VIEW_HEIGHT / 2 - (m_model->screenHeight() * scale) / 2 + MARGIN_H;

        pw->setMovedX((pw->pos().x() - offsetX) / scale);
        pw->setMovedY((pw->pos().y() - offsetY) / scale);
    }

    adjust(pw);
}

void MonitorsGround::adjust(MonitorProxyWidget *pw)
{
    const double scale = screenScale();

    const int offsetX = VIEW_WIDTH / 2 - (m_model->screenWidth() * scale) / 2;
    const int offsetY = VIEW_HEIGHT / 2 - (m_model->screenHeight() * scale) / 2;

    const int w = scale * pw->w();
    const int h = scale * pw->h();
    const int x = scale * pw->x();
    const int y = scale * pw->y();

    pw->setGeometry(x + offsetX + MARGIN_W, y + offsetY + MARGIN_H, w, h);
}

double MonitorsGround::screenScale() const
{
    const double scaleW = VIEW_WIDTH / m_model->screenWidth();
    const double scaleH = VIEW_HEIGHT / m_model->screenHeight();

    return std::min(scaleW, scaleH);
}
