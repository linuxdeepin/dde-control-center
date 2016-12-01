#include "recognizedialog.h"
#include "displaymodel.h"

#include <QTimer>
#include <QPainter>

const int AUTOHIDE_DELAY = 1000 * 5;

RecognizeDialog::RecognizeDialog(DisplayModel *model, QWidget *parent)
    : QDialog(parent),

      m_model(model)
{
    connect(m_model, &DisplayModel::screenHeightChanged, this, &RecognizeDialog::onScreenRectChanged);
    connect(m_model, &DisplayModel::screenWidthChanged, this, &RecognizeDialog::onScreenRectChanged);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    onScreenRectChanged();

    // set auto hide timer
    QTimer *autoHideTimer = new QTimer(this);
    autoHideTimer->setInterval(AUTOHIDE_DELAY);
    connect(autoHideTimer, &QTimer::timeout, this, &RecognizeDialog::accept);
    autoHideTimer->start();
}

void RecognizeDialog::paintEvent(QPaintEvent *)
{
    QPen p;
    p.setWidth(1);
    p.setColor(Qt::black);

    QBrush b;
    b.setColor(Qt::white);
    b.setStyle(Qt::SolidPattern);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(p);
    painter.setBrush(b);

    for (auto mon : m_model->monitorList())
        paintMonitorMark(painter, mon->rect(), mon->name());
}

void RecognizeDialog::onScreenRectChanged()
{
    setFixedWidth(m_model->screenWidth());
    setFixedHeight(m_model->screenHeight());
    move(0, 0);

    update();
}

void RecognizeDialog::paintMonitorMark(QPainter &painter, const QRect &rect, const QString &name)
{
    const int fontSize = rect.height() / 5;
    QFont font;
    font.setPixelSize(fontSize);
    const QFontMetrics fm(font);
    const int x = rect.center().x() - fm.width(name) / 2;
    const int y = rect.center().y() + fm.height() / 4;

    QPainterPath path;
    path.addText(x, y, font, name);
    painter.drawPath(path);
}
