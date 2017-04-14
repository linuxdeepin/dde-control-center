#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include <com_deepin_daemon_bluetooth.h>
#include <com_deepin_daemon_inputdevice_wacom.h>

using BluetoothInter = com::deepin::daemon::Bluetooth;
using WacomInter = com::deepin::daemon::inputdevice::Wacom;

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavWidget(QWidget *parent = 0);

signals:
    void requestModule(const QString &module) const;

private:
    void onDevicesChanged();
    void refershGridLayout();
    void setTipsText(const QString &text);

    void leaveEvent(QEvent *e);

private:
    QLabel *m_tipsLabel;
    QGridLayout *m_gridLayout;
    QTimer *m_deviceRefreshDelay;

    BluetoothInter *m_bluetoothInter;
    WacomInter *m_wacomInter;

    QStringList m_moduleList;
};

#endif // NAVWIDGET_H
