#include "miracastnodevicepage.h"
#include "labels/normallabel.h"
#include "dimagebutton.h"
#include <QVBoxLayout>
#include <QPainter>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::display;
using namespace dcc::widgets;

MiracastNoDevicePage::MiracastNoDevicePage(QWidget *parent) : TranslucentFrame(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    NormalLabel *tip = new NormalLabel(tr("No available device found"));

    DImageButton *refresh = new DImageButton;
    refresh->setText(tr("Refresh"));

    layout->addSpacing(30);
    layout->addWidget(tip, 0, Qt::AlignCenter);
    layout->addWidget(refresh, 0, Qt::AlignCenter);
    layout->addSpacing(20);

    setLayout(layout);
}

void dcc::display::MiracastNoDevicePage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(painter.renderHints() | QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 4, 4);

    painter.fillPath(path, QColor(254, 254, 254, 0.13 * 255));
}
