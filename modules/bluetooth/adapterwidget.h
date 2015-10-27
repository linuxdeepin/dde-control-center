#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include <QWidget>

#include <libdui/dlistwidget.h>
#include <libdui/dloadingindicator.h>
#include <libdui/dswitchbutton.h>

#include "bluetoothmainwidget.h"

DUI_USE_NAMESPACE

class NormalLabel;
class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(BluetoothMainWidget::AdapterInfo *info,
                           QWidget *parent = 0);
    ~AdapterWidget();

    void addDevice(BluetoothMainWidget::DeviceInfo *info);
    void removeDevice(BluetoothMainWidget::DeviceInfo *info);

public slots:
    void updateUI();

private:
    void initUI();

    DListWidget *m_deviceItemList = nullptr;
    BluetoothMainWidget::AdapterInfo *m_info = nullptr;
    NormalLabel *m_bluetoothName;
    DSwitchButton *m_bluetoothSwitch;
    DLoadingIndicator *m_refreshnndicator;
};

#endif // ADAPTERWIDGET_H
