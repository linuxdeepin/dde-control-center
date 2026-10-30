// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WIRELESSCASTINGMODEL_H
#define WIRELESSCASTINGMODEL_H

#include <QObject>
#include <QDBusMessage>
#include <QDBusObjectPath>

class QDBusVariant;
namespace Dtk {
namespace Core {
class DDBusInterface;
}
}

class Monitor : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        ND_SINK_STATE_DISCONNECTED       =      0x0,
        ND_SINK_STATE_ENSURE_FIREWALL    =     0x50,
        ND_SINK_STATE_WAIT_P2P           =    0x100,
        ND_SINK_STATE_WAIT_SOCKET        =    0x110,
        ND_SINK_STATE_WAIT_STREAMING     =    0x120,
        ND_SINK_STATE_STREAMING          =   0x1000,
        ND_SINK_STATE_ERROR              =  0x10000,
    } NdSinkState;

    explicit Monitor(const QString& monitorPath, QObject *parent = nullptr);
    ~Monitor() Q_DECL_OVERRIDE;

    void connMonitor();
    void disconnectMonitor();
    QString name() { return m_name; }
    QString hwAddress() { return m_name; }
    NdSinkState state() { return m_state; }

signals:
    void stateChanged(const NdSinkState state);
    void StatusChanged(int status);
    void NameChanged(const QString &name);
    void stateChangeFinished(int state);

private:
    void initData();
    void checkState(const QVariant &var);

    Dtk::Core::DDBusInterface *m_dbus;
    QString m_hwAddress;
    QString m_name;
    NdSinkState m_state;
    quint32 m_strength;
};

class WirelessCastingModel : public QObject
{
    Q_OBJECT
public:
    enum  CastingState{
        List,
        Connected,
        NoMonitor,
        NotSupportP2P,
        WarningInfo,
        DisabledWirelessDevice,
        NoWirelessDevice,
        Count
    };

    explicit WirelessCastingModel(QObject *parent = nullptr);
    ~WirelessCastingModel() Q_DECL_OVERRIDE;

    QMap<QString, Monitor*> monitors() { return m_monitors; }
    CastingState state() { return m_state; }
    void refresh();
    void disconnectMonitor();
    const QString curMonitorName();
    void enableRefresh(bool enable);
    bool connectState() const { return m_connecting; }
    bool multiscreensFlag() { return m_hasMultiscreens; }
    void setMultiscreensFlag(bool hasMultiscreens);

public slots:
    void handleMonitorStateChanged(const Monitor::NdSinkState state);
    void onDBusNameOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner);

signals:
    void addMonitor(const QString &path, Monitor *monitor);
    void removeMonitor(const QString &path);
    void stateChanged(CastingState state);
    void wirelessDevChanged(bool wirelessDevCheck);

    void SinkListChanged(const QList<QDBusObjectPath> &sinkList);
    void MissingCapabilitiesChanged(const QStringList &missingList);

    void AllDevicesChanged(const QList<QDBusObjectPath> &devList);
    void WirelessEnabledChanged(bool enabled);
    void DeviceEnabled(const QString &, bool);

    void connectStateChanged(bool connecting);

    void multiscreensFlagChanged(const bool flag);
    void stateChangeFinished(int state);

    void serviceAvailableChanged(bool available);

protected:
    void timerEvent(QTimerEvent *event) override;
private:
    void initData();
    void updateSinkList(const QVariant &sinkList);
    void updateWarningInfo(const QVariant &var);
    void checkState();
    void prepareDbus();
    void resetNetworkDisplayData();
    void setConnectState(bool connecting);
    void checkServiceAvailable();

    Dtk::Core::DDBusInterface *m_dbus;
    Dtk::Core::DDBusInterface *m_dbusNM;
    QMap<QString, Monitor*> m_monitors;
    CastingState m_state;
    Monitor *m_curConnected;
    bool m_wirelessDevCheck;
    bool m_wirelessEnabled;
    void setState(CastingState state);
    bool m_needRefresh;
    bool m_connecting;
    bool m_hasMultiscreens;
    bool m_serviceAvailable = false;
};
Q_DECLARE_METATYPE(WirelessCastingModel::CastingState)
#endif // WIRELESSCASTINGMODEL_H
