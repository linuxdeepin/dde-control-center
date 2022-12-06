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
#include "displaydbusproxy.h"
#include "widgets/dccdbusinterface.h"

#include <QDBusPendingReply>
#include <QDBusMetaType>

const static QString DisplayService = "org.deepin.dde.Display1";
const static QString DisplayPath = "/org/deepin/dde/Display1";
const static QString DisplayInterface = "org.deepin.dde.Display1";

const static QString AppearanceService = "org.deepin.dde.Appearance1";
const static QString AppearancePath = "/org/deepin/dde/Appearance1";
const static QString AppearanceInterface = "org.deepin.dde.Appearance1";

const static QString PowerService = "org.deepin.dde.Power1";
const static QString PowerPath = "/org/deepin/dde/Power1";
const static QString PowerInterface = "org.deepin.dde.Power1";

/* TODO
const static QString CooperationService = "org.deepin.dde.Cooperation1";
const static QString CooperationPath = "/org/deepin/dde/Cooperation1";
const static QString CooperationInterface = "org.deepin.dde.Cooperation1";
*/

const static QString CooperationService = "com.deepin.Cooperation";
const static QString CooperationPath = "/com/deepin/Cooperation";
const static QString CooperationInterface = "com.deepin.Cooperation";

DisplayDBusProxy::DisplayDBusProxy(QObject *parent)
    : QObject(parent)
{
    registerTouchscreenInfoList_V2MetaType();
    registerTouchscreenMapMetaType();
    registerResolutionListMetaType();
    registerBrightnessMapMetaType();
    registerTouchscreenInfoListMetaType();
    registerScreenRectMetaType();
    registerResolutionMetaType();

    init();
}

void DisplayDBusProxy::init()
{
    m_dBusSystemDisplayInter = new DCC_NAMESPACE::DCCDBusInterface("org.deepin.dde.Display1", "/org/deepin/dde/Display1", "org.deepin.dde.Display1", QDBusConnection::systemBus(), this);
    m_dBusDisplayInter = new DCC_NAMESPACE::DCCDBusInterface(DisplayService, DisplayPath, DisplayInterface, QDBusConnection::sessionBus(), this);
    m_dBusAppearanceInter = new DCC_NAMESPACE::DCCDBusInterface(AppearanceService, AppearancePath, AppearanceInterface, QDBusConnection::sessionBus(), this);
    m_dBusPowerInter = new DCC_NAMESPACE::DCCDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this);
    m_dBusCooperationInter = new DCC_NAMESPACE::DCCDBusInterface(CooperationService, CooperationPath, CooperationInterface, QDBusConnection::sessionBus(), this);
}

//power
bool DisplayDBusProxy::ambientLightAdjustBrightness()
{
    return qvariant_cast<bool>(m_dBusPowerInter->property("AmbientLightAdjustBrightness"));
}

void DisplayDBusProxy::setAmbientLightAdjustBrightness(bool value)
{
    m_dBusPowerInter->setProperty("AmbientLightAdjustBrightness", QVariant::fromValue(value));
}

bool DisplayDBusProxy::hasAmbientLightSensor()
{
    return qvariant_cast<bool>(m_dBusPowerInter->property("HasAmbientLightSensor"));
}

QList<QDBusObjectPath> DisplayDBusProxy::Machines()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_dBusCooperationInter->property("Machines"));
}

QList<QString> DisplayDBusProxy::CooperatedMachines()
{
    qDebug() << " CooperatedMachines:  " << qvariant_cast<QList<QString>>(m_dBusCooperationInter->property("CooperatedMachines"));
    return qvariant_cast<QList<QString>>(m_dBusCooperationInter->property("CooperatedMachines"));
}

bool DisplayDBusProxy::deviceSharingSwitch()
{
    return qvariant_cast<bool>(m_dBusCooperationInter->property("DeviceSharingSwitch"));
}

void DisplayDBusProxy::setDeviceSharingSwitch(const bool enable)
{
    qDebug() << " TODO： setDeviceSharingSwitch begin " << enable ;
    m_dBusCooperationInter->setProperty("DeviceSharingSwitch", QVariant::fromValue(enable));
}

void DisplayDBusProxy::setOpenSharedClipboard(bool on)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(on);
    m_dBusCooperationInter->asyncCallWithArgumentList(QStringLiteral("OpenSharedClipboard"), argumentList);
}

bool DisplayDBusProxy::SharedClipboard()
{
    return qvariant_cast<bool>(m_dBusCooperationInter->property("SharedClipboard"));
}

void DisplayDBusProxy::setFilesStoragePath(const QString &path)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(path);
    m_dBusCooperationInter->asyncCallWithArgumentList(QStringLiteral("SetFilesStoragePath"), argumentList);
}

QString DisplayDBusProxy::FilesStoragePath()
{
    return qvariant_cast<QString>(m_dBusCooperationInter->property("FilesStoragePath"));
}

void DisplayDBusProxy::setOpenSharedDevices(bool on)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(on);
    m_dBusCooperationInter->asyncCallWithArgumentList(QStringLiteral("OpenSharedDevices"), argumentList);
}

bool DisplayDBusProxy::SharedDevices()
{
    return qvariant_cast<bool>(m_dBusCooperationInter->property("SharedDevices"));
}

//display
BrightnessMap DisplayDBusProxy::brightness()
{
    return qvariant_cast<BrightnessMap>(m_dBusDisplayInter->property("Brightness"));
}

int DisplayDBusProxy::colorTemperatureManual()
{
    return qvariant_cast<int>(m_dBusDisplayInter->property("ColorTemperatureManual"));
}

int DisplayDBusProxy::colorTemperatureMode()
{
    return qvariant_cast<int>(m_dBusDisplayInter->property("ColorTemperatureMode"));
}

QString DisplayDBusProxy::currentCustomId()
{
    return qvariant_cast<QString>(m_dBusDisplayInter->property("CurrentCustomId"));
}

QStringList DisplayDBusProxy::customIdList()
{
    return qvariant_cast<QStringList>(m_dBusDisplayInter->property("CustomIdList"));
}

uchar DisplayDBusProxy::displayMode()
{
    return qvariant_cast<uchar>(m_dBusDisplayInter->property("DisplayMode"));
}
bool DisplayDBusProxy::hasChanged()
{
    return qvariant_cast<bool>(m_dBusDisplayInter->property("HasChanged"));
}

uint DisplayDBusProxy::maxBacklightBrightness()
{
    return qvariant_cast<uint>(m_dBusDisplayInter->property("MaxBacklightBrightness"));
}

QList<QDBusObjectPath> DisplayDBusProxy::monitors()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_dBusDisplayInter->property("Monitors"));
}

QString DisplayDBusProxy::primary()
{
    return qvariant_cast<QString>(m_dBusDisplayInter->property("Primary"));
}

ScreenRect DisplayDBusProxy::primaryRect()
{
    return qvariant_cast<ScreenRect>(m_dBusDisplayInter->property("PrimaryRect"));
}

ushort DisplayDBusProxy::screenHeight()
{
    return qvariant_cast<ushort>(m_dBusDisplayInter->property("ScreenHeight"));
}

ushort DisplayDBusProxy::screenWidth()
{
    return qvariant_cast<ushort>(m_dBusDisplayInter->property("ScreenWidth"));
}

TouchscreenMap DisplayDBusProxy::touchMap()
{
    return qvariant_cast<TouchscreenMap>(m_dBusDisplayInter->property("TouchMap"));
}

TouchscreenInfoList DisplayDBusProxy::touchscreens()
{
    return qvariant_cast<TouchscreenInfoList>(m_dBusDisplayInter->property("Touchscreens"));
}

TouchscreenInfoList_V2 DisplayDBusProxy::touchscreensV2()
{
    return qvariant_cast<TouchscreenInfoList_V2>(m_dBusDisplayInter->property("TouchscreensV2"));
}


QDBusPendingReply<double> DisplayDBusProxy::GetScaleFactor()
{
    QList<QVariant> argumentList;
    return m_dBusAppearanceInter->asyncCallWithArgumentList(QStringLiteral("GetScaleFactor"), argumentList);
}

QDBusPendingReply<QMap<QString,double> > DisplayDBusProxy::GetScreenScaleFactors()
{
    QList<QVariant> argumentList;
    return m_dBusAppearanceInter->asyncCallWithArgumentList(QStringLiteral("GetScreenScaleFactors"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetScaleFactor(double in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusAppearanceInter->asyncCallWithArgumentList(QStringLiteral("SetScaleFactor"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetScreenScaleFactors(const QMap<QString,double> &scaleFactors)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(scaleFactors);
    return m_dBusAppearanceInter->asyncCallWithArgumentList(QStringLiteral("SetScreenScaleFactors"), argumentList);
}

QString DisplayDBusProxy::GetConfig()
{
    QList<QVariant> argumentList;
    return QDBusPendingReply<QString>(m_dBusSystemDisplayInter->asyncCallWithArgumentList(QStringLiteral("GetConfig"), argumentList));
}

void DisplayDBusProxy::SetConfig(QString cfgStr)
{
    QList<QVariant> argumentList;
    argumentList << cfgStr;
    m_dBusSystemDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetConfig"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::ApplyChanges()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("ApplyChanges"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::AssociateTouch(const QString &in0, const QString &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("AssociateTouch"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::AssociateTouchByUUID(const QString &in0, const QString &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("AssociateTouchByUUID"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::ChangeBrightness(bool in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("ChangeBrightness"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::DeleteCustomMode(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("DeleteCustomMode"), argumentList);
}

QDBusPendingReply<uchar> DisplayDBusProxy::GetRealDisplayMode()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("GetRealDisplayMode"), argumentList);
}

QDBusPendingReply<QStringList> DisplayDBusProxy::ListOutputNames()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("ListOutputNames"), argumentList);
}

QDBusPendingReply<ResolutionList> DisplayDBusProxy::ListOutputsCommonModes()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("ListOutputsCommonModes"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::ModifyConfigName(const QString &in0, const QString &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("ModifyConfigName"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::RefreshBrightness()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("RefreshBrightness"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::Reset()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("Reset"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::ResetChanges()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("ResetChanges"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::Save()
{
    QList<QVariant> argumentList;
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("Save"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetAndSaveBrightness(const QString &in0, double in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetAndSaveBrightness"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetBrightness(const QString &in0, double in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetBrightness"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetColorTemperature(int in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetColorTemperature"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetMethodAdjustCCT(int in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetMethodAdjustCCT"), argumentList);
}

QDBusPendingReply<> DisplayDBusProxy::SetPrimary(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetPrimary"), argumentList);
}


QDBusPendingReply<> DisplayDBusProxy::SwitchMode(uchar in0, const QString &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SwitchMode"), argumentList);
}

QDBusReply<bool> DisplayDBusProxy::CanSetBrightnessSync(const QString &name)
{
    return m_dBusDisplayInter->call("CanSetBrightness", name);
}

QDBusReply<bool> DisplayDBusProxy::SupportSetColorTemperatureSync()
{
    return m_dBusDisplayInter->call("SupportSetColorTemperatureSync");
}
