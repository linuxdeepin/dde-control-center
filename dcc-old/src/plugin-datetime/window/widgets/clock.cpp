//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "clock.h"

#include <QPainter>
#include <QPainterPath>
#include <QIcon>
#include <QTimer>
#include <DIconTheme>

DGUI_USE_NAMESPACE

static const QSize clockSize = QSize(224, 224);
static const QSize pointSize = QSize(145, 15);

Clock::Clock(QWidget *parent)
    : QWidget(parent)
    , m_drawTicks(true)
    , m_autoNightMode(true)
    , n_bIsUseBlackPlat(true)
{
    m_hour = getPixmap("dcc_noun_hour", pointSize);
    m_min = getPixmap("dcc_noun_minute", pointSize);
    m_sec = getPixmap("dcc_noun_second", pointSize);
    setMinimumSize(clockSize);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Clock::update));
    timer->start(1000);
}

Clock::~Clock()
{
}

QPixmap Clock::getPixmap(const QString &name, const QSize size)
{
    const QIcon &icon = DIconTheme::findQIcon(name);
    const qreal ratio = devicePixelRatioF();
    QPixmap pixmap = icon.pixmap(size * ratio).scaled(size * ratio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPainter p(&pixmap);
    p.setRenderHints(QPainter::Antialiasing);
    p.drawPixmap(0, 0, pixmap);
    pixmap.setDevicePixelRatio(ratio);
    return pixmap;
}

void Clock::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QDateTime datetime(QDateTime::currentDateTime());
    const QTime time(datetime.time());
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    do {
        const bool nightMode = !(time.hour() >= 6 && time.hour() < 18);
        if (nightMode == m_isBlack && !m_plat.isNull())
            break;
        if (nightMode) {
            m_plat = getPixmap("dcc_clock_black", clockSize);
            m_isBlack = true;
        } else {
            m_plat = getPixmap("dcc_clock_white", clockSize);
            m_isBlack = false;
        }
    } while (false);

    // draw plate
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.drawPixmap(QPointF(-clockSize.width() / 2.0, -clockSize.height() / 2.0), m_plat);
    painter.restore();

    int nHour = (time.hour() >= 12) ? (time.hour() - 12) : time.hour();
    int nStartAngle = 90; //The image from 0 start , but the clock need from -90 start

    // draw hour hand
    const qreal hourAngle = qreal(nHour * 30 + time.minute() * 30 / 60 + time.second() * 30 / 60 / 60 - nStartAngle);
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(hourAngle);
    painter.drawPixmap(QPointF(-pointSize.width() / 2.0, -pointSize.height() / 2.0), m_hour);
    painter.restore();

    // draw minute hand
    const qreal minuteAngle = qreal(time.minute() * 6 + time.second() * 6 / 60 - nStartAngle);
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(minuteAngle);
    painter.drawPixmap(QPointF(-pointSize.width() / 2.0, -pointSize.height() / 2.0), m_min);
    painter.restore();

    // draw second hand
    const qreal secondAngle = qreal(time.second() * 6 - nStartAngle);
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(secondAngle);
    painter.drawPixmap(QPointF(-pointSize.width() / 2.0, -pointSize.height() / 2.0), m_sec);
    painter.restore();

    painter.end();
}
////////////////////////////////////////////////////////////
bool Clock::autoNightMode() const
{
    return m_autoNightMode;
}

void Clock::setAutoNightMode(bool autoNightMode)
{
    if (m_autoNightMode != autoNightMode) {
        m_autoNightMode = autoNightMode;
        update();
    }
}

void Clock::setPlate(bool isBlack)
{
    if (n_bIsUseBlackPlat != isBlack) {
        n_bIsUseBlackPlat = isBlack;
        update();
    }
}

void Clock::setTimeZone(const ZoneInfo &timeZone)
{
    if (m_timeZone == timeZone)
        return;

    m_timeZone = timeZone;
    update();
}

bool Clock::drawTicks() const
{
    return m_drawTicks;
}

void Clock::setDrawTicks(bool drawTicks)
{
    if (m_drawTicks != drawTicks) {
        m_drawTicks = drawTicks;
        update();
    }
}
