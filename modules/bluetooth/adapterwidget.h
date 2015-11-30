#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include <QWidget>

#include <libdui/dlistwidget.h>
#include <libdui/dloadingindicator.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/darrowlineexpand.h>

#include "bluetoothmainwidget.h"
#include "confrimwidget.h"

DUI_USE_NAMESPACE

class NormalLabel;
class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(BluetoothMainWidget::AdapterInfo *info,
                           QWidget *parent = 0);
    ~AdapterWidget();

    void removeConfirm(ConfrimWidget *confrim);
    void addConfirm(ConfrimWidget *confirm, BluetoothMainWidget::DeviceInfo *info);
    void addDevice(BluetoothMainWidget::DeviceInfo *info);
    void addTrustedDevice(BluetoothMainWidget::DeviceInfo *info);
    void removeDevice(BluetoothMainWidget::DeviceInfo *info, bool isDelete);
    void removeTrustedDevice(BluetoothMainWidget::DeviceInfo *info);

public slots:
    void updateUI();

private:
    void initUI();

    DListWidget *m_deviceItemList = nullptr;
    DSeparatorHorizontal *m_listWidgetSeparator = nullptr;
    BluetoothMainWidget::AdapterInfo *m_info = nullptr;
    NormalLabel *m_bluetoothName;
    DSwitchButton *m_bluetoothSwitch;
    DArrowLineExpand *m_activeDeviceExpand;
    DListWidget *m_activeDeviceList;
    DLoadingIndicator *m_refreshnndicator;
    QLabel *m_tipsLabel;
    DHeaderLine *m_headerLine;
    DSeparatorHorizontal *m_separator;
};

#endif // ADAPTERWIDGET_H
