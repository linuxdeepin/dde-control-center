#include <QDebug>
#include <QResizeEvent>
#include <QJsonDocument>
#include <QEventLoop>

#include <libdui/dconstants.h>
#include <libdui/dseparatorhorizontal.h>

#include "deviceitemwidget.h"
#include "bluetoothobject.h"
#include "adapterwidget.h"
#include "constants.h"

DUI_USE_NAMESPACE

BluetoothObject::BluetoothObject(QObject *parent) :
    QObject(parent)
{
    /// show later
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

BluetoothObject::~BluetoothObject()
{
    for(const AdapterInfo *info : m_pathToAdapterInfoMap.values()) {
        info->widget->deleteLater();
    }
}

QStringList BluetoothObject::getIds() const
{
    QStringList list;

    for(AdapterInfo *info : m_pathToAdapterInfoMap.values()) {
        list << info->path;
    }

    return list;
}

BluetoothObject::AdapterInfo* BluetoothObject::getAdapterInfoByPath(const QString &path) const
{
    return m_pathToAdapterInfoMap.value(path, nullptr);
}

bool BluetoothObject::dbusValid() const
{
    return m_bluetoothDbus->isValid();
}

void BluetoothObject::addAdapter(AdapterInfo *info)
{
    info->widget = new AdapterWidget(info);

    emit adapterAdd(info->path);
}

void BluetoothObject::removeAdapter(const AdapterInfo *info)
{
    if(!info)
        return;

    m_pathToAdapterInfoMap.remove(info->path);
    info->widget->deleteLater();

    emit adapterRemove(info->path);
}

BluetoothObject::AdapterInfo* BluetoothObject::newAdapterInfoByMap(const QVariantMap &map)
{
    AdapterInfo *info = new AdapterInfo;

    info->path = map["Path"].toString();
    info->bluetoothDbus = m_bluetoothDbus;

    m_pathToAdapterInfoMap[info->path] = info;

    updateAdapterInfoByMap(info, map);

    ASYN_CALL(m_bluetoothDbus->GetDevices(QDBusObjectPath(info->path)), {
                  if(!info->widget)
                    return;

                  QJsonDocument json_doc = QJsonDocument::fromJson(args[0].toByteArray());
                  for(const QJsonValue &value : json_doc.array()){
                      const QVariantMap &tmp_map = value.toObject().toVariantMap();
                      DeviceInfo *device_info = newDeviceInfoByMap(tmp_map);

                      device_info->item = newDeviceListItem(device_info);
                      info->widget->addDevice(device_info);
                  }
              }, this, info);

    return info;
}

BluetoothObject::DeviceInfo *BluetoothObject::newDeviceInfoByMap(const QVariantMap &map)
{
    DeviceInfo *info = new DeviceInfo;

    info->path = map["Path"].toString();

    m_pathToDeviceInfoMap[info->path] = info;

    updateDeviceInfoByMap(info, map);

    return info;
}

DeviceItemWidget *BluetoothObject::newDeviceListItem(DeviceInfo *device_info) const
{
    DeviceItemWidget *item = new DeviceItemWidget(device_info);
    item->setFixedSize(BLUETOOTH_PLUGIN_WIDTH, DUI::EXPAND_HEADER_HEIGHT);

    return item;
}

void BluetoothObject::updateAdapterInfoByMap(BluetoothObject::AdapterInfo *info, const QVariantMap &map)
{
    if(!info)
        return;

    info->name = map["Alias"].toString();
    info->powered = map["Powered"].toBool();
    info->discovering = map["Discovering"].toBool();
    info->discoverable = map["Discoverable"].toBool();
    info->discoverableTimeout = map["DiscoverableTimeout"].toUInt();
}

void BluetoothObject::updateDeviceInfoByMap(BluetoothObject::DeviceInfo *info, const QVariantMap &map)
{
    if(!info)
        return;

    info->name = map["Alias"].toString();
    info->trusted = map["Trusted"].toBool();
    info->paired = map["Paired"].toBool();
    info->state = map["State"].toInt();
    info->icon = map["Icon"].toString();
}

void BluetoothObject::init()
{
    m_bluetoothDbus = new DBusBluetooth(this);

    if(!m_bluetoothDbus->isValid()) {
        QTimer *timer = new QTimer(this);

        static int timerout_count = 0;

        connect(timer, &QTimer::timeout, [this, timer] {
            if(++timerout_count > 10) {
                timer->stop();
                timer->deleteLater();
                qWarning() << "DBusBluetooth is invaild.";

                return;
            }

            if(m_bluetoothDbus->isValid()) {
                timer->stop();
                timer->deleteLater();
                initBackend();
            }
        });

        timer->start(1500);
    }
}

QVariantMap getMapByJson(const QString &json)
{
    QJsonDocument json_doc = QJsonDocument::fromJson(json.toUtf8());
    return json_doc.object().toVariantMap();
}

void BluetoothObject::initBackend()
{
    if(m_bluetoothDbus->state() > 0) {
        ASYN_CALL(m_bluetoothDbus->GetAdapters(), {
                      QJsonDocument json_doc = QJsonDocument::fromJson(args[0].toByteArray());
                      for(const QJsonValue &value : json_doc.array()){
                          const QVariantMap &map = value.toObject().toVariantMap();
                          addAdapter(newAdapterInfoByMap(map));
                      }
                  }, this);
    }

    connect(m_bluetoothDbus, &DBusBluetooth::AdapterAdded, this, [this](const QString &str) {
        addAdapter(newAdapterInfoByMap(getMapByJson(str)));
    });

    connect(m_bluetoothDbus, &DBusBluetooth::AdapterRemoved, this, [this](const QString &str) {
        removeAdapter(m_pathToAdapterInfoMap.value(getMapByJson(str)["Path"].toString(), nullptr));
    });

    connect(m_bluetoothDbus, &DBusBluetooth::DeviceAdded, this, [this](const QString &str){
        const QVariantMap &map = getMapByJson(str);

        AdapterInfo *info = m_pathToAdapterInfoMap.value(map["AdapterPath"].toString(), nullptr);
        if(info){
            DeviceInfo *device_info = newDeviceInfoByMap(map);
            device_info->item = newDeviceListItem(device_info);
            info->widget->addDevice(device_info);
        }
    });

    connect(m_bluetoothDbus, &DBusBluetooth::DeviceRemoved, this, [this](const QString &str){
        const QVariantMap &map = getMapByJson(str);
        AdapterInfo *info = m_pathToAdapterInfoMap.value(map["AdapterPath"].toString(), nullptr);
        if(info){
            DeviceInfo *device_info = m_pathToDeviceInfoMap.value(map["Path"].toString(), nullptr);
            if(device_info){
                info->widget->removeDevice(device_info, true);
                m_pathToDeviceInfoMap.remove(device_info->path);
            }
        }
    });

    connect(m_bluetoothDbus, &DBusBluetooth::AdapterPropertiesChanged, this, [this](const QString &str){
        const QVariantMap &map = getMapByJson(str);
        AdapterInfo *info = m_pathToAdapterInfoMap.value(map["Path"].toString(), nullptr);
        updateAdapterInfoByMap(info, map);
        if(info) {
            info->widget->updateUI();
            emit adapterPropertyChanged(info->path);
        }
    });

    connect(m_bluetoothDbus, &DBusBluetooth::DevicePropertiesChanged, this, [this](const QString &str){
        const QVariantMap &map = getMapByJson(str);
        DeviceInfo *info = m_pathToDeviceInfoMap.value(map["Path"].toString(), nullptr);
        updateDeviceInfoByMap(info, map);
        if(info)
            info->item->updateUI();
    });
}
