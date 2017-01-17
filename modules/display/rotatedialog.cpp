#include "rotatedialog.h"
#include "displayworker.h"
#include "monitor.h"
#include "displaymodel.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QWindow>
#include <QX11Info>
#include <QPainter>
#include <QPaintEvent>

#include <xcb/xproto.h>

static void BlurWindowBackground(const WId windowId, const QRect &region)
{
    xcb_connection_t *connection = QX11Info::connection();
    const char *name = "_NET_WM_DEEPIN_BLUR_REGION";
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection,
                                      0,
                                      strlen(name),
                                      name);

    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection,
                                     cookie,
                                     NULL);
    if (reply) {
        const int data[] = {region.x(), region.y(), region.width(), region.height()};

        xcb_change_property_checked(connection,
                                    XCB_PROP_MODE_REPLACE,
                                    windowId,
                                    reply->atom,
                                    XCB_ATOM_CARDINAL,
                                    32,
                                    4,
                                    data);
        xcb_flush(connection);

        free(reply);
    }
}

using namespace dcc::display;

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent),

      m_mon(mon),
      m_model(nullptr)
{
    connect(m_mon, &Monitor::wChanged, this, &RotateDialog::setFixedWidth);
    connect(m_mon, &Monitor::hChanged, this, &RotateDialog::setFixedHeight);
    connect(m_mon, &Monitor::xChanged, this, [=] (const int x) { move(x, y()); });
    connect(m_mon, &Monitor::yChanged, this, [=] (const int y) { move(x(), y); });

    setFixedWidth(m_mon->w());
    setFixedHeight(m_mon->h());
    move(m_mon->x(), m_mon->y());

    init();
}

RotateDialog::RotateDialog(DisplayModel *model, QWidget *parent)
    : QDialog(parent),

      m_mon(nullptr),
      m_model(model)
{
    connect(m_model, &DisplayModel::screenWidthChanged, this, &RotateDialog::setFixedWidth);
    connect(m_model, &DisplayModel::screenHeightChanged, this, &RotateDialog::setFixedHeight);

    setFixedWidth(m_model->screenWidth());
    setFixedHeight(m_model->screenHeight());
    move(0, 0);

    init();
}

void RotateDialog::init()
{
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    qApp->setOverrideCursor(Qt::BlankCursor);
}

RotateDialog::~RotateDialog()
{
    qApp->restoreOverrideCursor();
}

void RotateDialog::mousePressEvent(QMouseEvent *e)
{
    QDialog::mousePressEvent(e);

    switch (e->button())
    {
    case Qt::RightButton:   accept();       break;
    case Qt::LeftButton:    rotate();       break;
    default:;
    }
}

void RotateDialog::mouseMoveEvent(QMouseEvent *e)
{
    QDialog::mouseMoveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::resizeEvent(QResizeEvent *e)
{
    const int l = 100;
    const QRect r((width() - l) / 2, (height() - l) / 2, l, l);

    BlurWindowBackground(winId(), r);

    QDialog::resizeEvent(e);
}

void RotateDialog::leaveEvent(QEvent *e)
{
    QDialog::leaveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::paintEvent(QPaintEvent *e)
{
    const QString Tips(tr("Left click to rotate and right click to exit"));

    QDialog::paintEvent(e);

    QFontMetrics fm(font());
    const int tw = fm.width(Tips);
    const int w = width();
    const int h = height();
    const int margin = 100;

    QPainter painter(this);
    painter.fillRect(rect(), QColor(127, 127, 127, 255 * .6));

    QPixmap rotatePixmap(":/display/themes/common/icon/rotate.png");
    painter.drawPixmap(rect().center() - rotatePixmap.rect().center(), rotatePixmap);

    // bottom
    painter.drawText((w - tw) / 2, h - margin, Tips);

    // left
    painter.rotate(90);
    painter.drawText((h - tw) / 2, -margin, Tips);

    // top
    painter.rotate(90);
    painter.drawText((w + tw) / -2, -margin, Tips);

    // right
    painter.rotate(90);
    painter.drawText((h + tw) / -2, w - margin, Tips);
}

void RotateDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    QTimer::singleShot(1, this, &RotateDialog::grabKeyboard);
}

void RotateDialog::rotate()
{
    Monitor *mon = m_mon ? m_mon : m_model->primaryMonitor();
    Q_ASSERT(mon);

    const auto rotates = mon->rotateList();
    const auto rotate = mon->rotate();

    Q_ASSERT(rotates.contains(rotate));

    const quint16 nextValue = rotates[(rotates.indexOf(rotate) + 1) % rotates.size()];

    if (m_mon)
    {
        emit requestRotate(m_mon, nextValue);
    }
    else
    {
        for (auto *m : m_model->monitorList())
            emit requestRotate(m, nextValue);
    }
}
