//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MONITORDBUSPROXY_H
#define MONITORDBUSPROXY_H

#include "types/resolutionlist.h"
#include "types/reflectlist.h"
#include "types/rotationlist.h"
#include "types/resolution.h"


#include <QObject>
#include <QDBusPendingReply>
#include <QDBusReply>

#include <DDBusInterface>

class QDBusMessage;

class MonitorDBusProxy : public QObject
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.deepin.dde.Display1.Monitor"; }

public:
    explicit MonitorDBusProxy(QString monitorPath, QObject *parent = nullptr);
    Q_PROPERTY(QStringList AvailableFillModes READ availableFillModes NOTIFY AvailableFillModesChanged)
    QStringList availableFillModes();

    Q_PROPERTY(Resolution BestMode READ bestMode NOTIFY BestModeChanged)
    Resolution bestMode();

    Q_PROPERTY(bool Connected READ connected NOTIFY ConnectedChanged)
    bool connected();

    Q_PROPERTY(QString CurrentFillMode READ currentFillMode WRITE setCurrentFillMode NOTIFY CurrentFillModeChanged)
    QString currentFillMode();
    void setCurrentFillMode(const QString &value);

    Q_PROPERTY(Resolution CurrentMode READ currentMode NOTIFY CurrentModeChanged)
    Resolution currentMode();

    Q_PROPERTY(uchar CurrentRotateMode READ currentRotateMode NOTIFY CurrentRotateModeChanged)
    uchar currentRotateMode();

    Q_PROPERTY(bool Enabled READ enabled NOTIFY EnabledChanged)
    bool enabled();

    Q_PROPERTY(ushort Height READ height NOTIFY HeightChanged)
    ushort height();

    Q_PROPERTY(QString Manufacturer READ manufacturer NOTIFY ManufacturerChanged)
    QString manufacturer();

    Q_PROPERTY(uint MmHeight READ mmHeight NOTIFY MmHeightChanged)
    uint mmHeight();

    Q_PROPERTY(uint MmWidth READ mmWidth NOTIFY MmWidthChanged)
    uint mmWidth();

    Q_PROPERTY(QString Model READ model NOTIFY ModelChanged)
    QString model();

    Q_PROPERTY(ResolutionList Modes READ modes NOTIFY ModesChanged)
    ResolutionList modes();

    Q_PROPERTY(QString Name READ name NOTIFY NameChanged)
    QString name();

    Q_PROPERTY(ushort Reflect READ reflect NOTIFY ReflectChanged)
    ushort reflect();

    Q_PROPERTY(ReflectList Reflects READ reflects NOTIFY ReflectsChanged)
    ReflectList reflects();

    Q_PROPERTY(double RefreshRate READ refreshRate NOTIFY RefreshRateChanged)
    double refreshRate();

    Q_PROPERTY(ushort Rotation READ rotation NOTIFY RotationChanged)
    ushort rotation();

    Q_PROPERTY(RotationList Rotations READ rotations NOTIFY RotationsChanged)
    RotationList rotations();

    Q_PROPERTY(ushort Width READ width NOTIFY WidthChanged)
    ushort width();

    Q_PROPERTY(short X READ x NOTIFY XChanged)
    short x();

    Q_PROPERTY(short Y READ y NOTIFY YChanged)
    short y();

private:
    void init();

public Q_SLOTS: // METHODS
    QDBusPendingReply<> Enable(bool in0);
    QDBusPendingReply<> SetMode(uint in0);
    QDBusPendingReply<> SetModeBySize(ushort in0, ushort in1);
    QDBusPendingReply<> SetPosition(short in0, short in1);
    QDBusPendingReply<> SetReflect(ushort in0);
    QDBusPendingReply<> SetRotation(ushort in0);

    void onPropertiesChanged(const QDBusMessage &message);

Q_SIGNALS: // SIGNALS
    // begin property changed signals
    void AvailableFillModesChanged(const QStringList & value) const;
    void BestModeChanged(Resolution  value) const;
    void ConnectedChanged(bool  value) const;
    void CurrentFillModeChanged(const QString & value) const;
    void CurrentModeChanged(Resolution  value) const;
    void CurrentRotateModeChanged(uchar  value) const;
    void EnabledChanged(bool  value) const;
    void HeightChanged(ushort  value) const;
    void ManufacturerChanged(const QString & value) const;
    void MmHeightChanged(uint  value) const;
    void MmWidthChanged(uint  value) const;
    void ModelChanged(const QString & value) const;
    void ModesChanged(ResolutionList  value) const;
    void NameChanged(const QString & value) const;
    void ReflectChanged(ushort  value) const;
    void ReflectsChanged(ReflectList  value) const;
    void RefreshRateChanged(double  value) const;
    void RotationChanged(ushort  value) const;
    void RotationsChanged(RotationList  value) const;
    void WidthChanged(ushort  value) const;
    void XChanged(short  value) const;
    void YChanged(short  value) const;

private:
    Dtk::Core::DDBusInterface *m_dBusMonitorInter;
    Dtk::Core::DDBusInterface *m_dBusMonitorPropertiesInter;
    QString m_monitorUserPath;
};

#endif // MONITORDBUSPROXY_H
