#ifndef DEVICELISTITEM_H
#define DEVICELISTITEM_H

#include <QWidget>

#include <libdui/dloadingindicator.h>

#include "genericlistitem.h"

#include "bluetoothmainwidget.h"

class DeviceListItem : public GenericListItem
{
    Q_OBJECT

    Q_PROPERTY(bool loading READ loading WRITE setLoading)

public:
    explicit DeviceListItem(BluetoothMainWidget::DeviceInfo *info, QWidget *parent = 0);
    ~DeviceListItem();

    bool loading() const;

public slots:
    void setLoading(bool loading);
    void updateUI();

private:
    void initUI();

    bool m_loading = false;
    DLoadingIndicator *m_indicator;
    BluetoothMainWidget::DeviceInfo *m_info = nullptr;
};

#endif // DEVICELISTITEM_H
