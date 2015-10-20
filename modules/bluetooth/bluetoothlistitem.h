#ifndef BLUETOOTHLISTITEM_H
#define BLUETOOTHLISTITEM_H

#include <QWidget>

#include <libdui/dloadingindicator.h>

#include "genericlistitem.h"

class BluetoothListItem : public GenericListItem
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading)

public:
    explicit BluetoothListItem(QWidget *parent = 0);
    bool loading() const;

public slots:
    void setLoading(bool loading);

private:
    bool m_loading = false;
    DLoadingIndicator *m_indicator;
};

#endif // BLUETOOTHLISTITEM_H
