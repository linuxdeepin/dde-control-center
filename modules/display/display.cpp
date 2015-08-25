#include <QDebug>
#include <QFrame>
#include <QDBusConnection>

#include "display.h"
#include "singlemonitorsettings.h"

Display::Display():
    QObject(),
    m_frame(NULL)
{
    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_dbusDisplay = new DisplayInterface(this);

    m_frame = new SingleMonitorSettings(m_dbusDisplay, m_dbusDisplay->monitors().at(0).path());
    m_frame->setFixedWidth(310);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

Display::~Display()
{
    qDebug() << "~Display()";
    delete m_frame ;
}

QFrame *Display::getContent()
{
    return m_frame;
}

