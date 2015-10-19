#include <QLabel>
#include <QDebug>

#include "bluetoothmainwidget.h"

#include "bluetooth.h"

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

    m_mainWidget->deleteLater();
}

QFrame* Bluetooth::getContent()
{
    return m_mainWidget;
}
