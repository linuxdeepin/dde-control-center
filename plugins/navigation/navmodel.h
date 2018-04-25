#ifndef NAVMODEL_H
#define NAVMODEL_H

#include <QAbstractTableModel>
#include <com_deepin_daemon_bluetooth.h>
#include <com_deepin_daemon_inputdevice_wacom.h>

using BluetoothInter = com::deepin::daemon::Bluetooth;
using WacomInter = com::deepin::daemon::inputdevice::Wacom;

const QStringList MODULES = { "accounts", "display", "defapp", "personalization", "network",
                              "bluetooth", "sound", "datetime", "power", "mouse", "keyboard",
                              "wacom", "update", "systeminfo" };

class NavModel : public QAbstractTableModel
{
public:
    NavModel(QObject *parent = Q_NULLPTR);

public:
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void removeModule(const QModelIndex &index);
    void removeModule(const QString &moduleName);
    void addModule(const QString &moduleName);

public:
    QString transModuleName(const QString &moduleName);

private Q_SLOTS:
    void onBTStateChanged(uint value);
    void onWacomExistChanged(bool value);

private:
    QStringList validModuleList();

private:
    BluetoothInter *m_bluetoothInter;
    WacomInter *m_wacomInter;

    QStringList m_moduleList;
};

#endif // NAVMODEL_H
