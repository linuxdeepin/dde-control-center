#include <QLabel>
#include <QSvgWidget>

#include <dde-dock/dockpluginproxyinterface.h>

#include "bluetoothplugin.h"
#include "bluetoothobject.h"
#include "adapterwidget.h"

BluetoothPlugin::BluetoothPlugin(QObject *parent) :
    QObject(parent)
{
    m_settings = new QSettings("deepin", "dde-dock-bluetooth-plugin", this);

    qDebug() << "Bluetooth: BluetoothPlugin created";
}

BluetoothPlugin::~BluetoothPlugin()
{
    qDeleteAll(m_mapIdToButton.values());
}

QString BluetoothPlugin::getPluginName()
{
    return "Bluetooth plugin";
}

void BluetoothPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;

    qDebug() << "Bluetooth: BluetoothPlugin init";

    if(!m_bluetooth) {
        m_bluetooth = new BluetoothObject(this);

        connect(m_bluetooth, &BluetoothObject::adapterAdd, this, [this](const QString &path) {
            addItem(path);

            BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(path);

            if(info) {
                connect(info->widget, &AdapterWidget::sizeChanged, this, [this, path] {
                    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeAppletSize, path);
                });
            }

            m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, path);
        });

        connect(m_bluetooth, &BluetoothObject::adapterRemove, this, [this](const QString &path) {
            removeItem(path);
            m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, path);
        });

        connect(m_bluetooth, &BluetoothObject::adapterPropertyChanged, this, [this](const QString &path) {
            QSvgWidget *widget = m_mapIdToButton.value(path, nullptr);

            if(widget) {
                BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(path);

                const QString icon_path = QString(":/dark/images/bluetooth_%1.svg").arg(info && info->powered ? "on" : "off");

                widget->load(icon_path);
            }
        });
    }
}

void BluetoothPlugin::removeItem(const QString &id)
{
    m_proxy->itemRemovedEvent(id);

    QSvgWidget *label = m_mapIdToButton.value(id, nullptr);
    if(label)
        label->deleteLater();

    m_mapIdToButton.remove(id);
}

void BluetoothPlugin::addItem(const QString &path)
{
    QSvgWidget *widget = m_mapIdToButton.value(path, nullptr);

    if(!widget) {
        widget = new QSvgWidget;

        BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(path);

        const QString icon_path = QString(":/dark/images/bluetooth_%1.svg").arg(info && info->powered ? "on" : "off");

        widget->load(icon_path);
        widget->resize(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
        m_mapIdToButton[path] = widget;
    }

    m_proxy->itemAddedEvent(path);
}

void BluetoothPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    for (const QString& id : ids()) {
        if (newMode == Dock::FashionMode) {
            removeItem(id);
        } else if(oldMode == Dock::FashionMode){
            addItem(id);
        }

        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
    }
}

QStringList BluetoothPlugin::ids()
{
    return m_bluetooth->getIds();
}

QString BluetoothPlugin::getName(QString id)
{
    BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(id);

    return info ? info->name : "";
}

QString BluetoothPlugin::getTitle(QString id)
{
    return getName(id);
}

QString BluetoothPlugin::getCommand(QString)
{
    return "dde-control-center bluetooth";
}

QPixmap BluetoothPlugin::getIcon(QString id)
{
    Q_UNUSED(id)

    QPixmap icon(":/dark/images/bluetooth_on.svg");

    return icon.scaled(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
}

bool BluetoothPlugin::configurable(const QString &id)
{
    Q_UNUSED(id)

    return m_proxy->dockMode() != Dock::FashionMode ? m_mapIdToButton.contains(id) : false;
}

QString BluetoothPlugin::settingEnabledKey(const QString &id)
{
    return QString("%1_%2_enabled").arg(id).arg(m_proxy->dockMode());
}

bool BluetoothPlugin::enabled(const QString &id)
{
    return  m_proxy->dockMode() != Dock::FashionMode
            ? m_settings->value(settingEnabledKey(id), true).toBool()
            : false;    //default enabled
}

void BluetoothPlugin::setEnabled(const QString &id, bool enabled)
{
    m_settings->setValue(settingEnabledKey(id), enabled);

    if(enabled) {
        addItem(id);
    } else {
        removeItem(id);
    }
}

QWidget *BluetoothPlugin::getItem(QString id)
{
    return enabled(id) ? m_mapIdToButton.value(id, nullptr) : nullptr;
}

QWidget *BluetoothPlugin::getApplet(QString id)
{
    BluetoothObject::AdapterInfo *info = m_bluetooth->getAdapterInfoByPath(id);

    return info ? info->widget : NULL;
}

QString BluetoothPlugin::getMenuContent(QString id)
{
    return id;
}

void BluetoothPlugin::invokeMenuItem(QString, QString, bool)
{
}

