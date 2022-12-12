#ifndef MACHINEDBUSPROXY_H
#define MACHINEDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

class MachineDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MachineDBusProxy(QString cooperationMachinePath, QObject *parent = nullptr);

    Q_PROPERTY(QString IP READ IP NOTIFY IpChanged)
    QString IP();

    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

    Q_PROPERTY(bool Connected READ connected NOTIFY ConnectedChanged)
    bool connected();

    Q_PROPERTY(QString UUID READ UUID NOTIFY UuidChanged)
    QString UUID();

    Q_PROPERTY(bool Cooperating READ cooperating NOTIFY CooperatingChanged)
    bool cooperating(); // 默认设备

    Q_PROPERTY(int Direction READ direction NOTIFY directionChanged)
    int direction();

    void Disconnect();
    void Connect();
    void RequestCooperate();
    void SetFlowDirection(int direction);

Q_SIGNALS:
    void IpChanged(const QString& ip);
    void NameChanged(const QString& name);
    void ConnectedChanged(bool connecteded);
    void UuidChanged(const QString& uuid);
    void CooperatingChanged(bool cooperating);
    void disconnectStatusChanged(bool);
    void directionChanged(int direction);

private:
    void init();

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_dBusMachineInter;
    QString m_cooperationMachinePath;
};

#endif // MACHINEDBUSPROXY_H
