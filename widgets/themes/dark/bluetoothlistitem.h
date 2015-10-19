#ifndef BLUETOOTHLISTITEM_H
#define BLUETOOTHLISTITEM_H

#include "genericlistitem.h"

class BluetoothListItem : public GenericListItem
{
    Q_OBJECT
public:
    explicit BluetoothListItem(QWidget *parent = 0);

};

#endif // BLUETOOTHLISTITEM_H
