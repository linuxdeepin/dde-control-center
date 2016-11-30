#include "frame.h"
#include "settingswidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QScreen>
#include <QWindow>
#include <QX11Info>

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
    if (reply)
    {
        const int data[] = {region.x(), region.y(), region.width(), region.height()};

        xcb_change_property(connection,
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

#define BUTTON_LEFT 1

Frame::Frame(QWidget *parent)
    : QFrame(parent),

      m_allSettingsPage(nullptr),

      m_mouseAreaInter(new XMouseArea("com.deepin.api.XMouseArea", "/com/deepin/api/XMouseArea", QDBusConnection::sessionBus(), this)),
      m_displayInter(new DBusDisplay("com.deepin.daemon.Display", "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)),

      m_autoHide(true)
{
    m_displayInter->setSync(false);

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    //    setWindowFlags(Qt::X11BypassWindowManagerHint);
    //    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(360);

    connect(m_mouseAreaInter, &XMouseArea::ButtonRelease, this, &Frame::onMouseButtonReleased);
    connect(m_displayInter, &DBusDisplay::PrimaryRectChanged, this, &Frame::onScreenRectChanged);

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Frame::startup()
{
    show();
}

void Frame::pushWidget(ContentWidget *const w)
{
    Q_ASSERT(!m_frameWidgetStack.empty());

    FrameWidget *fw = new FrameWidget(this);
    fw->setContent(w);
    fw->show();

    m_frameWidgetStack.last()->hide();
    m_frameWidgetStack.push(fw);

    connect(w, &ContentWidget::back, this, &Frame::popWidget, Qt::UniqueConnection);
    connect(fw, &FrameWidget::contentDetached, this, &Frame::contentDetached, Qt::UniqueConnection);
}

void Frame::popWidget()
{
    Q_ASSERT(m_frameWidgetStack.size() > 1);

    // destory the container
    m_frameWidgetStack.pop()->destory();
    m_frameWidgetStack.last()->showBack();
}

void Frame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    QPalette pl(palette());
    QColor bgColor(Qt::black);
    bgColor.setAlphaF(0.9);

    painter.fillRect(event->rect(), bgColor);

    painter.end();
}

void Frame::resizeEvent(QResizeEvent *event)
{
#ifndef QT_DEBUG
    const QSize size(event->size());
    const QRect region(QPoint(0, 0), size);
    BlurWindowBackground(winId(), region);
#else
    Q_UNUSED(BlurWindowBackground);
#endif

    QFrame::resizeEvent(event);
}

void Frame::init()
{
    // main page
    MainWidget *w = new MainWidget(this);
    w->setVisible(false);

    connect(w, &MainWidget::showAllSettings, this, &Frame::showAllSettings);

    m_frameWidgetStack.push(w);

    // frame position adjust
    onScreenRectChanged(m_displayInter->primaryRect());

#ifdef QT_DEBUG
    showSettingsPage("sound", QString());
#endif
}

void Frame::setAutoHide(const bool autoHide)
{
    m_autoHide = autoHide;
}

void Frame::showAllSettings()
{
    if (!m_allSettingsPage)
    {
        m_allSettingsPage = new SettingsWidget(this);

        connect(m_allSettingsPage, &SettingsWidget::requestAutohide, this, &Frame::setAutoHide);
    }

    pushWidget(m_allSettingsPage);
}

void Frame::showSettingsPage(const QString &moduleName, const QString &pageName)
{
    // ensure current is main page
    Q_ASSERT(m_frameWidgetStack.size() == 1);

    // load all settings page
    showAllSettings();

    // show specificed page
    m_allSettingsPage->showModulePage(moduleName, pageName);
}

void Frame::contentDetached(QWidget *const c)
{
    ContentWidget *cw = qobject_cast<ContentWidget *>(c);
    Q_ASSERT(cw);

    if (cw != m_allSettingsPage)
        return m_allSettingsPage->contentPopuped(cw);

    // delete all settings panel
    m_allSettingsPage->deleteLater();
    m_allSettingsPage = nullptr;
}

void Frame::onScreenRectChanged(const QRect &primaryRect)
{
    // pass invalid data
    if (primaryRect.isEmpty())
        return;

    setFixedHeight(primaryRect.height());
    QFrame::move(primaryRect.right() - width() + 1, primaryRect.y());
}

void Frame::onMouseButtonReleased(const int button, const int x, const int y, const QString &key)
{
    if (key != m_mouseAreaKey)
        return;

    if (button != BUTTON_LEFT)
        return;

    const QPoint p(pos());

    if (rect().contains(x - p.x(), y - p.y()))
        return;

    if (!m_autoHide)
        return;

    // ready to hide frame
    hide();
}

void Frame::keyPressEvent(QKeyEvent *e)
{
    QFrame::keyPressEvent(e);

    switch (e->key())
    {
#ifdef QT_DEBUG
    case Qt::Key_Escape:
        qApp->quit();
        break;
    case Qt::Key_F1:
        hide();
        break;
#endif
    default:;
    }
}

void Frame::show()
{
    QFrame::show();
    QTimer::singleShot(0, this, [=] { m_frameWidgetStack.last()->show(); });

    // register global mouse area
    m_mouseAreaKey = m_mouseAreaInter->RegisterFullScreen();
}

void Frame::hide()
{
    m_autoHide = true;

    FrameWidget *w = m_frameWidgetStack.last();

    w->hideBack();

    QTimer::singleShot(w->animationDuration(), this, &QFrame::hide);

    // unregister global mouse area
    m_mouseAreaInter->UnregisterArea(m_mouseAreaKey);
}
