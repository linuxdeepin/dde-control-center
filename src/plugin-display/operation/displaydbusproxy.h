/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     Tinalu Shao <shaotianlu@uniontech.com>
*
* Maintainer: Tinalu Shao <shaotianlu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef DISPLAYDBUSPROXY_H
#define DISPLAYDBUSPROXY_H

#include "types/touchscreeninfolist_v2.h"
#include "types/touchscreenmap.h"
#include "types/resolutionlist.h"
#include "types/brightnessmap.h"
#include "types/touchscreeninfolist.h"
#include "types/screenrect.h"

#include <QObject>
#include <QDBusPendingReply>
#include <QDBusReply>

class QDBusInterface;
class QDBusMessage;

class DisplayDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit DisplayDBusProxy(QObject *parent = nullptr);
    Q_PROPERTY(BrightnessMap Brightness READ brightness NOTIFY BrightnessChanged)
    BrightnessMap brightness();

    Q_PROPERTY(int ColorTemperatureManual READ colorTemperatureManual NOTIFY ColorTemperatureManualChanged)
    int colorTemperatureManual();

    Q_PROPERTY(int ColorTemperatureMode READ colorTemperatureMode NOTIFY ColorTemperatureModeChanged)
    int colorTemperatureMode();

    Q_PROPERTY(QString CurrentCustomId READ currentCustomId NOTIFY CurrentCustomIdChanged)
    QString currentCustomId();

    Q_PROPERTY(QStringList CustomIdList READ customIdList NOTIFY CustomIdListChanged)
    QStringList customIdList();

    Q_PROPERTY(uchar DisplayMode READ displayMode NOTIFY DisplayModeChanged)
    uchar displayMode();

    Q_PROPERTY(bool HasChanged READ hasChanged NOTIFY HasChangedChanged)
    bool hasChanged();

    Q_PROPERTY(uint MaxBacklightBrightness READ maxBacklightBrightness NOTIFY MaxBacklightBrightnessChanged)
    uint maxBacklightBrightness();

    Q_PROPERTY(QList<QDBusObjectPath> Monitors READ monitors NOTIFY MonitorsChanged)
    QList<QDBusObjectPath> monitors();

    Q_PROPERTY(QString Primary READ primary NOTIFY PrimaryChanged)
    QString primary();

    Q_PROPERTY(ScreenRect PrimaryRect READ primaryRect NOTIFY PrimaryRectChanged)
    ScreenRect primaryRect();

    Q_PROPERTY(ushort ScreenHeight READ screenHeight NOTIFY ScreenHeightChanged)
    ushort screenHeight();

    Q_PROPERTY(ushort ScreenWidth READ screenWidth NOTIFY ScreenWidthChanged)
    ushort screenWidth();

    Q_PROPERTY(TouchscreenMap TouchMap READ touchMap NOTIFY TouchMapChanged)
    TouchscreenMap touchMap();

    Q_PROPERTY(TouchscreenInfoList Touchscreens READ touchscreens NOTIFY TouchscreensChanged)
    TouchscreenInfoList touchscreens();

    Q_PROPERTY(TouchscreenInfoList_V2 TouchscreensV2 READ touchscreensV2 NOTIFY TouchscreensV2Changed)
    TouchscreenInfoList_V2 touchscreensV2();

    //power
    Q_PROPERTY(bool AmbientLightAdjustBrightness READ ambientLightAdjustBrightness WRITE setAmbientLightAdjustBrightness NOTIFY AmbientLightAdjustBrightnessChanged)
    bool ambientLightAdjustBrightness();
    void setAmbientLightAdjustBrightness(bool value);

    Q_PROPERTY(bool HasAmbientLightSensor READ hasAmbientLightSensor NOTIFY HasAmbientLightSensorChanged)
    bool hasAmbientLightSensor();

private:
    void init();

public Q_SLOTS: // METHODS
    //Display
    QDBusPendingReply<> ApplyChanges();
    QDBusPendingReply<> AssociateTouch(const QString &in0, const QString &in1);
    QDBusPendingReply<> AssociateTouchByUUID(const QString &in0, const QString &in1);
    QDBusPendingReply<> ChangeBrightness(bool in0);
    QDBusPendingReply<> DeleteCustomMode(const QString &in0);
    QDBusPendingReply<uchar> GetRealDisplayMode();
    QDBusPendingReply<QStringList> ListOutputNames();
    QDBusPendingReply<ResolutionList> ListOutputsCommonModes();
    QDBusPendingReply<> ModifyConfigName(const QString &in0, const QString &in1);
    QDBusPendingReply<> RefreshBrightness();
    QDBusPendingReply<> Reset();
    QDBusPendingReply<> ResetChanges();
    QDBusPendingReply<> Save();
    QDBusPendingReply<> SetAndSaveBrightness(const QString &in0, double in1);
    QDBusPendingReply<> SetBrightness(const QString &in0, double in1);
    QDBusPendingReply<> SetColorTemperature(int in0);
    QDBusPendingReply<> SetMethodAdjustCCT(int in0);
    QDBusPendingReply<> SetPrimary(const QString &in0);
    QDBusPendingReply<> SwitchMode(uchar in0, const QString &in1);
    QDBusReply<bool> CanSetBrightnessSync(const QString &name);
    QDBusReply<bool> SupportSetColorTemperatureSync();
    //Appearance
    QDBusPendingReply<double> GetScaleFactor();
    QDBusPendingReply<QMap<QString,double> > GetScreenScaleFactors();
    QDBusPendingReply<> SetScaleFactor(double in0);
    QDBusPendingReply<> SetScreenScaleFactors(const QMap<QString,double> &scaleFactors);


    void onPropertiesChanged(const QDBusMessage &message);

Q_SIGNALS: // SIGNALS
    // begin property changed signals
    void BrightnessChanged(BrightnessMap  value) const;
    void ColorTemperatureManualChanged(int  value) const;
    void ColorTemperatureModeChanged(int  value) const;
    void CurrentCustomIdChanged(const QString & value) const;
    void CustomIdListChanged(const QStringList & value) const;
    void DisplayModeChanged(uchar  value) const;
    void HasChangedChanged(bool  value) const;
    void MaxBacklightBrightnessChanged(uint  value) const;
    void MonitorsChanged(const QList<QDBusObjectPath> & value) const;
    void PrimaryChanged(const QString & value) const;
    void PrimaryRectChanged(ScreenRect  value) const;
    void ScreenHeightChanged(ushort  value) const;
    void ScreenWidthChanged(ushort  value) const;
    void TouchMapChanged(TouchscreenMap  value) const;
    void TouchscreensChanged(TouchscreenInfoList  value) const;
    void TouchscreensV2Changed(TouchscreenInfoList_V2  value) const;

    //power
    void AmbientLightAdjustBrightnessChanged(bool  value) const;
    void HasAmbientLightSensorChanged(bool  value) const;

private:
    QDBusInterface *m_dBusDisplayInter;
    QDBusInterface *m_dBusAppearanceInter;
    QDBusInterface *m_dBusPowerInter;

    QDBusInterface *m_dBusDisplayPropertiesInter;
};

#endif // DISPLAYDBUSPROXY_H
