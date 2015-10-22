#ifndef ADAPTERWIDGET_H
#define ADAPTERWIDGET_H

#include <QWidget>

#include <libdui/dlistwidget.h>

#include "bluetoothmainwidget.h"

DUI_USE_NAMESPACE

class AdapterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AdapterWidget(BluetoothMainWidget::AdapterInfo *info,
                           QWidget *parent = 0);

    DListWidget *deviceList() const;

private:
    void initUI(BluetoothMainWidget::AdapterInfo *info);

    DListWidget *m_deviceList = nullptr;
    BluetoothMainWidget::AdapterInfo *m_info;
};

#endif // ADAPTERWIDGET_H
