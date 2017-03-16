#include "themeitempic.h"

#include <QPainter>
#include <QPainterPath>

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

ThemeItemPic::ThemeItemPic(QWidget *parent)
    :QWidget(parent)
{
    setFixedSize(320, 70);
}

void ThemeItemPic::setPicPath(const QString &path)
{
    m_picPath = path;
    update();
}

void ThemeItemPic::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);

    painter.save();

    painter.setClipPath(path);
    QPixmap pix(m_picPath);
    painter.drawPixmap(rect(), pix);

    painter.restore();

    painter.end();
}
