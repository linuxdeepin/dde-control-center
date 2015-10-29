#ifndef DEVICELISTITEM_H
#define DEVICELISTITEM_H

#include <QWidget>

#include "genericlistitem.h"

#include "bluetoothmainwidget.h"

class DeviceListItem : public GenericListItem
{
    Q_OBJECT

public:
    explicit DeviceListItem(BluetoothMainWidget::DeviceInfo *info, QWidget *parent = 0);
    ~DeviceListItem();

public slots:
    void updateUI();

private:
    void initUI();

    BluetoothMainWidget::DeviceInfo *m_info = nullptr;
};

#endif // DEVICELISTITEM_H
