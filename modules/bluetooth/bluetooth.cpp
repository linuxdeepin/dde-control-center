#include <QLabel>
#include <QDebug>

#include "bluetoothmainwidget.h"
#include "bluetooth.h"
#include "dbus/dbusbluetooth.h"

Bluetooth::Bluetooth()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Bluetooth"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_mainWidget = new BluetoothMainWidget;
}

Bluetooth::~Bluetooth()
{
    qDebug() << "~Bluetooth()";

    m_mainWidget->hide();
    m_mainWidget->setParent(nullptr);
    delete m_mainWidget;
}

QFrame* Bluetooth::getContent()
{
    return m_mainWidget;
}

void Bluetooth::setProxy(ControlCenterProxyInterface *proxy)
{
    ModuleInterface::setProxy(proxy);

    DBusBluetooth *inter = new DBusBluetooth(this);
    connect(m_controlCenterProxy->dccObject(), SIGNAL(visibleChanged(bool)), inter, SLOT(ClearUnpairedDevice()));
}
