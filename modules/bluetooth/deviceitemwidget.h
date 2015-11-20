#ifndef DEVICEITEMWIDGET_H
#define DEVICEITEMWIDGET_H

#include <QWidget>

#include "bluetoothmainwidget.h"

#include <libdui/dimagebutton.h>
#include <dloadingindicator.h>

DUI_USE_NAMESPACE

class DeviceItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceItemWidget(BluetoothMainWidget::DeviceInfo *info, QWidget *parent = 0);
    ~DeviceItemWidget();

public slots:
    void updateUI();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private:
    void disConnect();

private:
    BluetoothMainWidget::DeviceInfo *m_info = nullptr;

    QLabel *m_deviceName;
    QLabel *m_statTips;
    DImageButton *m_removeBtn;
    DLoadingIndicator *m_loadingIndicator;
};

#endif // DEVICEITEMWIDGET_H
