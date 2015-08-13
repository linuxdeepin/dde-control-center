#include <QDebug>
#include <QFrame>
#include <QDBusConnection>

#include "display.h"
#include "singlemonitorsettings.h"

Display::Display():
    m_frame(NULL)
{
    m_dbusDisplay = new com::deepin::daemon::Display("com.deepin.daemon.Display",
                                                     "/com/deepin/daemon/Display",
                                                     QDBusConnection::sessionBus());

    m_frame = new SingleMonitorSettings(m_dbusDisplay->monitors().at(0).path());
    m_frame->setFixedWidth(300);
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

