#include <QIcon>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include <QDBusConnection>

#include <dde-dock/dockconstants.h>

#include "bluetoothcomponent.h"
#include "widgets/switchbutton.h"

BluetoothComponent::BluetoothComponent(QString id, QObject *parent) :
    QObject(parent),
    m_item(NULL),
    m_applet(NULL),
    m_id(id)
{
    QIcon::setThemeName("Deepin");

    m_item = new QLabel;
    m_item->setFixedSize(Dock::APPLET_CLASSIC_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);

    m_dbusBluetooth = new com::deepin::daemon::Bluetooth("com.deepin.daemon.Bluetooth",
                                                         "/com/deepin/daemon/Bluetooth",
                                                         QDBusConnection::sessionBus());
    this->initAdaptors();

    connect(m_dbusBluetooth, &Bluetooth::AdapterAdded, this, &BluetoothComponent::onAdaptorAdded);
    connect(m_dbusBluetooth, &Bluetooth::AdapterRemoved, this, &BluetoothComponent::onAdaptorRemoved);
    connect(m_dbusBluetooth, &Bluetooth::AdapterPropertiesChanged, this, &BluetoothComponent::onAdaptorPropertiesChanged);
}

BluetoothComponent::~BluetoothComponent()
{
    delete m_applet;
}

QPixmap BluetoothComponent::getIcon()
{
    return QIcon::fromTheme("bluetooth-active-symbolic").pixmap(QSize(Dock::APPLET_CLASSIC_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE));
}

QString BluetoothComponent::getName()
{
    return "Bluetooth";
}

QString BluetoothComponent::getTitle()
{
    return getName();
}

QString BluetoothComponent::getCommand()
{
    return "dde-control-center bluetooth";
}

QWidget * BluetoothComponent::getItem()
{
    if (m_adaptors.isEmpty()) {
        return NULL;
    } else {
        return m_item;
    }
}

void BluetoothComponent::retainItem()
{
    m_item->setParent(NULL);
}

QWidget * BluetoothComponent::getApplet()
{
    updateApplet();

    return m_applet;
}

QString BluetoothComponent::getMenuContent()
{
    return "";
}

void BluetoothComponent::invokeMenuItem(QString, bool)
{

}


// private methods
void BluetoothComponent::initAdaptors()
{
    QString adaptorsJsonString = m_dbusBluetooth->adapters();
    QJsonArray adaptors = QJsonDocument::fromJson(adaptorsJsonString.toUtf8()).array();

    foreach (QJsonValue value, adaptors) {
        QJsonObject adaptorJson = value.toObject();

        this->addAdaptor(adaptorJson);
    }
}

bool BluetoothComponent::isAnyAdaptorOn()
{
    foreach (BluetoothAdaptor adaptor, m_adaptors) {
        if (adaptor.powered) {
            return true;
        }
    }

    return false;
}

void BluetoothComponent::updateItem()
{
    if (isAnyAdaptorOn()) {
        m_item->setPixmap(QIcon::fromTheme("bluetooth-active-symbolic").pixmap(m_item->size()));
    } else {
        m_item->setPixmap(QIcon::fromTheme("bluetooth-disable-symbolic").pixmap(m_item->size()));
    }
}

void BluetoothComponent::updateApplet()
{
    if (!m_applet) {
        m_applet = new QFrame;
        new QVBoxLayout(m_applet);
    } else {
        QList<QFrame*> children = m_applet->findChildren<QFrame*>();
        QVBoxLayout * layout = qobject_cast<QVBoxLayout*>(m_applet->layout());

        if (layout) {
            foreach (QFrame * w, children) {
                layout->removeWidget(w);
                w->deleteLater();
            }
        }
    }

    QVBoxLayout * mainLayout = qobject_cast<QVBoxLayout*>(m_applet->layout());

    if (mainLayout) {
        foreach (BluetoothAdaptor adaptor, m_adaptors) {
            QLabel * label = new QLabel;
            label->setText(adaptor.alias);
            label->setStyleSheet("QLabel { color: white }");

            SwitchButton * switchButton = new SwitchButton;
            switchButton->setChecked(adaptor.powered);

            QFrame * lineFrame = new QFrame;

            QHBoxLayout * layout = new QHBoxLayout(lineFrame);
            layout->addWidget(label);
            layout->addStretch();
            layout->addWidget(switchButton);

            lineFrame->adjustSize();

            mainLayout->addWidget(lineFrame);

            connect(switchButton, &SwitchButton::toggled, [=](bool checked) {
                QDBusObjectPath path(adaptor.path);
                m_dbusBluetooth->SetAdapterPowered(path, checked);
                m_dbusBluetooth->SetAdapterDiscoverable(path, checked);
            });
        }

        m_applet->adjustSize();
    }
}

void BluetoothComponent::addAdaptor(QJsonObject json)
{
    BluetoothAdaptor adaptor;
    adaptor.path = json["Path"].toString();
    adaptor.alias = json["Alias"].toString();
    adaptor.powered = json["Powered"].toBool();
    adaptor.discovering = json["Discovering"].toBool();
    adaptor.discoverable = json["Discoverable"].toBool();
    adaptor.discoverableTimeout = json["DiscoverableTimeout"].toInt();

    m_adaptors << adaptor;

    this->updateItem();
}

void BluetoothComponent::removeAdaptor(QJsonObject json)
{
    for (int i = 0; i < m_adaptors.size(); ++i) {
        BluetoothAdaptor adaptor = m_adaptors.at(i);
        if (adaptor.path == json["Path"].toString()) {
            m_adaptors.removeAt(i);
        }
    }

    this->updateItem();
}

void BluetoothComponent::updateAdaptor(QJsonObject json)
{
    foreach (BluetoothAdaptor adaptor, m_adaptors) {
        if (adaptor.path == json["Path"].toString()) {
            adaptor.alias = json["Alias"].toString();
            adaptor.powered = json["Powered"].toBool();
            adaptor.discovering = json["Discovering"].toBool();
            adaptor.discoverable = json["Discoverable"].toBool();
            adaptor.discoverableTimeout = json["DiscoverableTimeout"].toInt();
        }
    }

    this->updateItem();
}

// private slots
void BluetoothComponent::onAdaptorAdded(QString adaptor)
{
    qDebug() << "Bluetooth adaptor added: " << adaptor;

    QJsonObject adaptorJson = QJsonDocument::fromJson(adaptor.toUtf8()).object();

    this->addAdaptor(adaptorJson);
}

void BluetoothComponent::onAdaptorRemoved(QString adaptor)
{
    qDebug() << "Bluetooth adaptor removed: " << adaptor;

    QJsonObject adaptorJson = QJsonDocument::fromJson(adaptor.toUtf8()).object();

    this->removeAdaptor(adaptorJson);
}

void BluetoothComponent::onAdaptorPropertiesChanged(QString adaptor)
{
    qDebug() << "Bluetooth adaptor properties changed: " << adaptor;

    QJsonObject adaptorJson = QJsonDocument::fromJson(adaptor.toUtf8()).object();

    this->updateAdaptor(adaptorJson);
}
