#include "frame.h"
#include "settingswidget.h"

#include <QScreen>
#include <QApplication>
#include <QKeyEvent>

Frame::Frame(QWidget *parent)
    : QMainWindow(parent),

      m_allSettingsPage(nullptr),

      m_displayInter(new Display("com.deepin.daemon.Display", "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this))
{
    m_displayInter->setSync(false);

    setWindowFlags(Qt::FramelessWindowHint);
//    setWindowFlags(Qt::X11BypassWindowManagerHint);
//    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(360);

    connect(m_displayInter, &Display::PrimaryRectChanged, this, &Frame::onScreenRectChanged);

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Frame::startup()
{
    show();
}

void Frame::pushWidget(ContentWidget * const w)
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

void Frame::init()
{
    // main page
    MainWidget *w = new MainWidget(this);
    w->setVisible(true);

    connect(w, &MainWidget::showAllSettings, this, &Frame::showAllSettings);

    m_frameWidgetStack.push(w);

    // frame position adjust
    onScreenRectChanged(m_displayInter->primaryRect());
}

void Frame::showAllSettings()
{
    if (!m_allSettingsPage)
        m_allSettingsPage = new SettingsWidget(this);

    pushWidget(m_allSettingsPage);
}

void Frame::contentDetached(QWidget * const c)
{
    ContentWidget *cw = qobject_cast<ContentWidget *>(c);
    if (cw && cw != m_allSettingsPage)
        m_allSettingsPage->contentPopuped(cw);
}

void Frame::onScreenRectChanged(const QRect &primaryRect)
{
    // pass invalid data
    if (primaryRect.isEmpty())
        return;

    setFixedHeight(primaryRect.height());
    QMainWindow::move(primaryRect.topLeft());
}

void Frame::keyPressEvent(QKeyEvent *e)
{
    QMainWindow::keyPressEvent(e);

    switch (e->key())
    {
#ifdef QT_DEBUG
    case Qt::Key_Escape:    qApp->quit();       break;
#endif
    default:;
    }
}
