#include "frame.h"
#include "settingswidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QScreen>

Frame::Frame(QWidget *parent)
    : BlurredFrame(parent),

      m_allSettingsPage(nullptr),

      m_mouseAreaInter(new XMouseArea("com.deepin.api.XMouseArea", "/com/deepin/api/XMouseArea", QDBusConnection::sessionBus(), this)),
      m_displayInter(new DBusDisplay("com.deepin.daemon.Display", "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)),
      m_launcherInter(new LauncherInter("com.deepin.dde.Launcher", "/com/deepin/dde/Launcher", QDBusConnection::sessionBus(), this)),

      m_primaryRect(m_displayInter->primaryRect()),
      m_appearAnimation(this, "geometry"),

      m_autoHide(true)
{
    // set async
    m_displayInter->setSync(false);

    m_appearAnimation.setEasingCurve(QEasingCurve::OutCubic);

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMaximumWidth(FRAME_WIDTH);

    connect(m_mouseAreaInter, &XMouseArea::ButtonRelease, this, &Frame::onMouseButtonReleased);
    connect(m_displayInter, &DBusDisplay::PrimaryRectChanged, this, &Frame::onScreenRectChanged);
    connect(m_launcherInter, &LauncherInter::Shown, this, &Frame::hide);

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

    // ensure top widget is sender()
    ContentWidget *w = static_cast<ContentWidget *>(sender());
    Q_ASSERT(w == m_frameWidgetStack.last()->content());

    // destory the container
    m_frameWidgetStack.pop()->destory();
    m_frameWidgetStack.last()->showBack();
}

void Frame::init()
{
    // main page
    MainWidget *w = new MainWidget(this);
    connect(w, &MainWidget::showAllSettings, this, &Frame::showAllSettings);
    connect(w, &MainWidget::showSettingPage, this, &Frame::showSettingsPage);
    m_frameWidgetStack.push(w);

    // frame position adjust
    onScreenRectChanged(m_primaryRect);

#ifdef QT_DEBUG
//    showSettingsPage("network", QString());
#endif
}

void Frame::setAutoHide(const bool autoHide)
{
    m_autoHide = autoHide;
}

void Frame::showAllSettings()
{
    Q_ASSERT(m_frameWidgetStack.size() == 1);

    if (!m_allSettingsPage) {
        m_allSettingsPage = new SettingsWidget(this);

        connect(m_allSettingsPage, &SettingsWidget::requestAutohide, this, &Frame::setAutoHide);
    }

    pushWidget(m_allSettingsPage);
}

void Frame::showSettingsPage(const QString &moduleName, const QString &pageName)
{
    // ensure current is main page or all settings page
    if (m_frameWidgetStack.size() > 2)
        popWidget();

    // current is main page
    if (m_frameWidgetStack.size() == 1)
        showAllSettings();

    // show specificed page
    m_allSettingsPage->showModulePage(moduleName, pageName);

    if (m_appearAnimation.startValue().toRect().width() != 0)
        show();
}

void Frame::contentDetached(QWidget *const c)
{
    ContentWidget *cw = qobject_cast<ContentWidget *>(c);
    Q_ASSERT(cw);

    if (cw != m_allSettingsPage) {
        return m_allSettingsPage->contentPopuped(cw);
    }

    // delete all settings panel
    m_allSettingsPage->deleteLater();
    m_allSettingsPage = nullptr;
}

void Frame::onScreenRectChanged(const QRect &primaryRect)
{
    // pass invalid data
    if (primaryRect.isEmpty()) {
        return;
    }

    m_primaryRect = primaryRect;

    setFixedHeight(m_primaryRect.height());
    QFrame::move(m_primaryRect.right() - width() + 1, m_primaryRect.y());
}

void Frame::onMouseButtonReleased(const int button, const int x, const int y, const QString &key)
{
    if (button != BUTTON_LEFT) {
        return;
    }

    if (!m_autoHide) {
        return;
    }

    if (key != m_mouseAreaKey) {
        return;
    }

    const QPoint p(pos());
    if (rect().contains(x - p.x(), y - p.y())) {
        return;
    }

    // ready to hide frame
    hide();
}

void Frame::keyPressEvent(QKeyEvent *e)
{
    QFrame::keyPressEvent(e);

    switch (e->key()) {
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
    if (m_appearAnimation.state() == QPropertyAnimation::Running)
        return;

    // animation
    QRect r = m_primaryRect;
    r.setLeft(m_primaryRect.x() + m_primaryRect.width());
    m_appearAnimation.setStartValue(r);
    r.setLeft(m_primaryRect.x() + m_primaryRect.width() - FRAME_WIDTH);
    m_appearAnimation.setEndValue(r);
    m_appearAnimation.start();

    // show frame
    QFrame::show();

    // notify top widget appear
    if (m_frameWidgetStack.last() && m_frameWidgetStack.last()->content())
    {
        QTimer::singleShot(m_frameWidgetStack.last()->animationDuration(), [=] {
            emit m_frameWidgetStack.last()->content()->appear();
        });
    }

    // register global mouse area
    m_mouseAreaKey = m_mouseAreaInter->RegisterFullScreen();
}

void Frame::hide()
{
    // reset auto-hide
    m_autoHide = true;

    // animation
    QRect r = m_primaryRect;
    r.setLeft(m_primaryRect.x() + m_primaryRect.width());
    m_appearAnimation.setStartValue(geometry());
    m_appearAnimation.setEndValue(r);
    m_appearAnimation.start();

//    QTimer::singleShot(m_appearAnimation.duration(), this, &QFrame::hide);
    QTimer::singleShot(m_appearAnimation.duration(), [=] {
        if (m_appearAnimation.state() != QPropertyAnimation::Running)
            QFrame::hide();
    });

    // notify top widget disappear
    if (m_frameWidgetStack.last() && m_frameWidgetStack.last()->content())
        emit m_frameWidgetStack.last()->content()->disappear();

    // unregister global mouse area
    m_mouseAreaInter->UnregisterArea(m_mouseAreaKey);
}

void Frame::toggle()
{
    if (m_appearAnimation.endValue().toRect().width() != 0)
        hide();
    else
        show();
}
