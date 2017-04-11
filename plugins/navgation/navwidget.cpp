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
    m_tipsLabel = new QLabel;
    m_tipsLabel->setStyleSheet("QLabel {"
                               "color: white;"
                               "background-color: rgba(255, 255, 255, .2);"
                               "}");
    m_tipsLabel->setAlignment(Qt::AlignCenter);
    m_tipsLabel->setVisible(false);
    m_gridLayout = new QGridLayout;
    m_gridLayout->setSpacing(1);
    m_gridLayout->setMargin(0);
    m_deviceRefreshDelay->setSingleShot(true);
    m_deviceRefreshDelay->setInterval(100);

    QVBoxLayout *centralLayout = new QVBoxLayout;

    centralLayout->addWidget(m_tipsLabel);
    centralLayout->addLayout(m_gridLayout);
    centralLayout->setSpacing(1);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);

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

    int index = 0;
    const int s = m_moduleList.size();
    for (int i = 0; i != s; ++i, ++index)
    {
        NavItemWidget *item = new NavItemWidget(m_moduleList[i]);

        connect(item, &NavItemWidget::itemClicked, this, &NavWidget::requestModule);

        m_gridLayout->addWidget(item, index / 3, index % 3);
    }

    const int r = s % 3;
    const int a = r == 1 ? 2 : r == 2 ? 1 : 0;
    for (int i = 0; i != a; ++i, ++index)
    {
        NavItemWidget *item = new NavItemWidget(QString());
        m_gridLayout->addWidget(item, index / 3, index % 3);
    }
}

void NavWidget::setTipsText(const QString &text)
{
    m_tipsLabel->setText(text);
}
