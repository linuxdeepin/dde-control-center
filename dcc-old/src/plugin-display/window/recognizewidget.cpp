//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "recognizewidget.h"
#include "operation/displaymodel.h"

#include <QPainter>
#include <QPainterPath>

const int FontSize = 20;
const int Radius = 18;
const int MiniWidth = 200;
const int VerticalMargin = 12;
const int HorizentalMargin = 22;
const int yoffset = 220;

using namespace DCC_NAMESPACE;
RecognizeWidget::RecognizeWidget(Monitor *monitor, QString text, QWidget *parent)
    : DBlurEffectWidget(parent), m_monitor(monitor), m_text(text)
{
    connect(m_monitor, &Monitor::geometryChanged, this, &RecognizeWidget::onScreenRectChanged);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setRadius(Radius);
    setMinimumWidth(MiniWidth);
    onScreenRectChanged();
    show();
    startTimer(5000);
}

void RecognizeWidget::paintEvent(QPaintEvent *event)
{
    DBlurEffectWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font;
    font.setStyle(QFont::StyleNormal);
    font.setPixelSize(FontSize);
    const QFontMetrics fm(font);

    QPainterPath path;
    path.addText((m_rect.width() - fm.horizontalAdvance(m_text)) / 2, m_rect.height() - VerticalMargin - fm.height() / 4, font, m_text);

    QPalette palette;
    QColor brushCorlor;
    brushCorlor = palette.color(QPalette::BrightText);

    painter.setPen(Qt::NoPen);
    painter.setBrush(brushCorlor);
    painter.drawPath(path);
}

void RecognizeWidget::onScreenRectChanged()
{
    const auto ratio = devicePixelRatioF();
    QRect displayRect(m_monitor->x(), m_monitor->y(), m_monitor->w(), m_monitor->h());
    displayRect = QRect(displayRect.topLeft(), displayRect.size() / ratio);

    QFont font;
    font.setStyle(QFont::StyleNormal);
    font.setPixelSize(FontSize);
    const QFontMetrics fm(font);
    int width = fm.horizontalAdvance(m_text) + 2 * HorizentalMargin > MiniWidth ? fm.horizontalAdvance(m_text) + 2 * HorizentalMargin : MiniWidth;
    int height = fm.height() + 2 * VerticalMargin;

    const int x = displayRect.center().x() - width / 2;
    const int y = displayRect.y() + displayRect.height() - height - yoffset;
    m_rect = QRect(x, y, width, height);

    setGeometry(m_rect);
    update();
}

void RecognizeWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    hide();
}
