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

void Frame::pushWidget(FrameWidget * const w)
{
    Q_ASSERT(!m_frameWidgetStack.empty());

    m_frameWidgetStack.last()->hide();
    m_frameWidgetStack.push(w);
    w->show();
}

void Frame::popWidget()
{
    Q_ASSERT(m_frameWidgetStack.size() > 1);

    m_frameWidgetStack.pop()->hide();
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
    {
        SettingsWidget *settingsWidget = new SettingsWidget(this);
        QVBoxLayout *settingsLayout = new QVBoxLayout;
        settingsLayout->addWidget(settingsWidget);
        settingsLayout->setSpacing(0);
        settingsLayout->setMargin(0);

        m_allSettingsPage = new FrameWidget(this);
        m_allSettingsPage->setLayout(settingsLayout);

        connect(settingsWidget, &SettingsWidget::back, this, &Frame::popWidget);
    }

    pushWidget(m_allSettingsPage);
}
