#include "frame.h"
#include "settingswidget.h"

#include <QScreen>
#include <QApplication>

Frame::Frame(QWidget *parent)
    : QMainWindow(parent),

      m_allSettingsPage(nullptr)
{
//    setWindowFlags(Qt::X11BypassWindowManagerHint);
//    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 600);
    move(qApp->primaryScreen()->geometry().center() - rect().center());

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
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
