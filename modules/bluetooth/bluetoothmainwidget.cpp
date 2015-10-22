#include <QDebug>
#include <QResizeEvent>
#include <QJsonDocument>

#include <libdui/dconstants.h>
#include <libdui/dseparatorhorizontal.h>

#include "moduleheader.h"
#include "constants.h"

#include "bluetoothlistitem.h"
#include "bluetoothmainwidget.h"
#include "adapterwidget.h"

DUI_USE_NAMESPACE

#define ASYN_CALL(Fun, Code, captured...) { \
    QDBusPendingCallWatcher * watcher = new QDBusPendingCallWatcher(Fun, this); \
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [watcher, captured]{ \
        const QVariantList & args = watcher->reply().arguments(); \
        Code \
        watcher->deleteLater(); \
    }); }

BluetoothMainWidget::BluetoothMainWidget(QWidget *parent) :
    QFrame(parent),
    m_mainLayout(new QVBoxLayout(this))
{
    initUI();
    intiBackend();
}

void BluetoothMainWidget::addAdapter(AdapterInfo *info)
{
    m_adapterList->addWidget(new AdapterWidget(info));
}

void BluetoothMainWidget::removeAdapter(const AdapterInfo *info)
{

}

void BluetoothMainWidget::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_adapterList->setFixedHeight(e->size().height() - m_adapterList->y());
}

BluetoothMainWidget::AdapterInfo* BluetoothMainWidget::newAdapterInfoByMap(const QVariantMap &map)
{
    AdapterInfo *info = new AdapterInfo;

    info->path = map["Path"].toString();
    info->name = map["Alias"].toString();
    info->powered = map["Powered"].toBool();
    info->discovering = map["Discovering"].toBool();
    info->discoverable = map["Discoverable"].toBool();
    info->discoverableTimeout = map["DiscoverableTimeout"].toUInt();
    info->bluetoothDbus = m_bluetoothDbus;

    ASYN_CALL(m_bluetoothDbus->GetDevices(QDBusObjectPath(info->path)), {
                  QJsonDocument json_doc = QJsonDocument::fromJson(args[0].toByteArray());
                  for(const QJsonValue &value : json_doc.array()){
                      const QVariantMap &tmp_map = value.toObject().toVariantMap();
                      DeviceInfo *device_info = newDeviceInfoByMap(tmp_map);
                      device_info->adapterInfo = info;
                      info->deviceInfoList.append(device_info);

                      if(info->listWidget){
                          device_info->listItem = newDeviceListItem(device_info);
                          info->listWidget->deviceList()->addWidget(device_info->listItem);
                      }
                  }
              }, this, info);

    return info;
}

BluetoothMainWidget::DeviceInfo *BluetoothMainWidget::newDeviceInfoByMap(const QVariantMap &map) const
{
    DeviceInfo *info = new DeviceInfo;

    info->path = map["Path"].toString();
    info->name = map["Alias"].toString();
    info->trusted = map["Trusted"].toBool();
    info->paired = map["Paired"].toBool();
    info->state = map["State"].toInt();
    info->icon = map["Icon"].toString();

    return info;
}

BluetoothListItem *BluetoothMainWidget::newDeviceListItem(DeviceInfo *device_info) const
{
    BluetoothListItem *item = new BluetoothListItem;
    item->setFixedSize(DCC::ModuleContentWidth, DUI::EXPAND_HEADER_HEIGHT);
    item->setTitle(device_info->name);

    connect(item, &BluetoothListItem::checkedChanged, this, [item](bool checked){
        item->setLoading(checked);
    });

    return item;
}

void BluetoothMainWidget::intiBackend()
{
    m_bluetoothDbus = new DBusBluetooth(this);

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
        QJsonDocument json_doc = QJsonDocument::fromJson(str.toLatin1());
        addAdapter(newAdapterInfoByMap(json_doc.toVariant().toMap()));
    });

    connect(m_bluetoothDbus, &DBusBluetooth::AdapterRemoved, this, [this](const QString &str) {
        QJsonDocument json_doc = QJsonDocument::fromJson(str.toLatin1());
        removeAdapter(newAdapterInfoByMap(json_doc.toVariant().toMap()));
    });

    connect(m_bluetoothDbus, &DBusBluetooth::DeviceAdded, this, [this](const QString &str){
        QJsonDocument json_doc = QJsonDocument::fromJson(str.toLatin1());
        const QVariantMap &map = json_doc.toVariant().toMap();
        DeviceInfo *device_info = newDeviceInfoByMap(map);
        device_info->adapterInfo = m_pathToAdapterInfoMap.value(map["AdapterPath"].toString(), nullptr);

        if(device_info->adapterInfo){
            device_info->listItem = newDeviceListItem(device_info);
            device_info->adapterInfo->listWidget->deviceList()->addWidget(device_info->listItem);
        }
    });

    connect(m_bluetoothDbus, &DBusBluetooth::DeviceRemoved, this, [this](const QString &str){
        QJsonDocument json_doc = QJsonDocument::fromJson(str.toLatin1());
        const QVariantMap &map = json_doc.toVariant().toMap();
        AdapterInfo *info = m_pathToAdapterInfoMap.value(map["AdapterPath"].toString(), nullptr);
        if(info){
            DeviceInfo *device_info = m_pathToDeviceInfoMap.value(map["Path"].toString(), nullptr);
            if(device_info){
                info->deviceInfoList.removeOne(device_info);
                DListWidget *device_list = info->listWidget->deviceList();
                int index = device_list->indexOf(device_info->listItem);
                if(index > 0)
                    device_list->removeWidget(index);
            }
        }
    });
}

GenericListItem *getListItem(const QString &name)
{
    GenericListItem *item = new GenericListItem;
    item->setTitle(name);
    item->setShowBgColor(false);

    return item;
}

void BluetoothMainWidget::initUI()
{
    ModuleHeader *header = new ModuleHeader(tr("Bluetooth"), false);

    m_adapterList = new DListWidget;

    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);

    m_mainLayout->addWidget(header);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
    m_mainLayout->addWidget(m_adapterList);
    m_mainLayout->addStretch(1);
}

