#include "navwidget.h"
#include "navitemwidget.h"

#include <QTimer>

const QStringList MODULES = { "accounts", "display", "defapp", "personalization", "network", "bluetooth", "sound", "datetime", "power", "mouse", "keyboard", "wacom", "update", "systeminfo" };

NavWidget::NavWidget(QWidget *parent)
    : QWidget(parent),

      m_deviceRefreshDelay(new QTimer(this)),
      m_bluetoothInter(new BluetoothInter("com.deepin.daemon.Bluetooth", "/com/deepin/daemon/Bluetooth", QDBusConnection::sessionBus(), this)),
      m_wacomInter(new WacomInter("com.deepin.daemon.InputDevices", "/com/deepin/daemon/InputDevice/Wacom", QDBusConnection::sessionBus(), this))
{
    m_gridLayout = new QGridLayout;
    m_deviceRefreshDelay->setSingleShot(true);
    m_deviceRefreshDelay->setInterval(100);

    setLayout(m_gridLayout);

    connect(m_deviceRefreshDelay, &QTimer::timeout, this, &NavWidget::onDevicesChanged);
    connect(m_bluetoothInter, &BluetoothInter::StateChanged, m_deviceRefreshDelay, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_wacomInter, &WacomInter::ExistChanged, m_deviceRefreshDelay, static_cast<void (QTimer::*)()>(&QTimer::start));

    m_deviceRefreshDelay->start();
}

void NavWidget::onDevicesChanged()
{
    Q_ASSERT(sender() == m_deviceRefreshDelay);

    m_moduleList = MODULES;

    if (!m_bluetoothInter->state())
        m_moduleList.removeOne("bluetooth");

    if (!m_wacomInter->exist())
        m_moduleList.removeOne("wacom");

    QTimer::singleShot(1, this, &NavWidget::refershGridLayout);
}

void NavWidget::refershGridLayout()
{
    // clear
    while (QLayoutItem *item = m_gridLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    for (int i = 0; i != m_moduleList.size(); ++i)
    {
        NavItemWidget *item = new NavItemWidget(m_moduleList[i]);

        connect(item, &NavItemWidget::itemClicked, this, &NavWidget::requestModule);

        m_gridLayout->addWidget(item, i / 3, i % 3);
    }
}
