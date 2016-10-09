#include "frame.h"
#include "settingswidget.h"

#include <QScreen>
#include <QApplication>

Frame::Frame(QWidget *parent)
    : QMainWindow(parent),

      m_allSettingsPage(nullptr)
{
    setFixedSize(300, 600);
    move(qApp->primaryScreen()->geometry().center() - rect().center());

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Frame::pushWidget(ContentWidget * const w)
{
    Q_ASSERT(!m_frameWidgetStack.empty());

    QVBoxLayout *fl = new QVBoxLayout;
    fl->addWidget(w);
    fl->setSpacing(0);
    fl->setMargin(0);
    FrameWidget *fw = new FrameWidget(this);
    fw->setLayout(fl);

    m_frameWidgetStack.last()->hide();
    m_frameWidgetStack.push(fw);
    fw->show();

    connect(w, &ContentWidget::back, this, &Frame::popWidget, Qt::UniqueConnection);
}

void Frame::popWidget()
{
    Q_ASSERT(m_frameWidgetStack.size() > 1);

    FrameWidget *fw = m_frameWidgetStack.pop();
    fw->layout()->takeAt(0)->widget()->setParent(nullptr);
    fw->hide();
    fw->deleteLater();

    m_frameWidgetStack.last()->show();
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
