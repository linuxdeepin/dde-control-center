#include "themeitempic.h"

#include <QPainter>
#include <QPainterPath>

using namespace dcc;
using namespace dcc::personalization;
using namespace dcc::widgets;

ThemeItemPic::ThemeItemPic(const QString &path, QWidget *parent)
    :QWidget(parent),
      m_picPath(path)
{
    setFixedSize(320, 70);
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
