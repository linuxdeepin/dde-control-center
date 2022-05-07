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

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

const static QString DisplayService = "com.deepin.daemon.Display";
const static QString DisplayPath = "/com/deepin/daemon/Display";
const static QString DisplayInterface = "com.deepin.daemon.Display";

const static QString AppearanceService = "com.deepin.daemon.Appearance";
const static QString AppearancePath = "/com/deepin/daemon/Appearance";
const static QString AppearanceInterface = "com.deepin.daemon.Appearance";

const static QString PowerService = "com.deepin.daemon.Power";
const static QString PowerPath = "/com/deepin/daemon/Power";
const static QString PowerInterface = "com.deepin.daemon.Power";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

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
    m_dBusDisplayPropertiesInter = new QDBusInterface(DisplayService, DisplayPath, PropertiesInterface, QDBusConnection::sessionBus(), this);
    m_dBusDisplayInter = new QDBusInterface(DisplayService, DisplayPath, DisplayInterface, QDBusConnection::sessionBus(), this);
    m_dBusAppearanceInter = new QDBusInterface(AppearanceService, AppearancePath, AppearanceInterface, QDBusConnection::sessionBus(), this);
    m_dBusPowerInter = new QDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this);

    QDBusConnection dbusConnection = m_dBusDisplayInter->connection();
    dbusConnection.connect(DisplayService, DisplayPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    dbusConnection = m_dBusPowerInter->connection();
    dbusConnection.connect(PowerService, PowerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
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

//display
BrightnessMap DisplayDBusProxy::brightness()
{
    return qvariant_cast<BrightnessMap>(m_dBusDisplayPropertiesInter->call("Get", DisplayInterface, "Brightness").arguments().at(0).value<QDBusVariant>().variant());
}

int DisplayDBusProxy::colorTemperatureManual()
{
    return qvariant_cast<int>(m_dBusDisplayInter->property("ColorTemperatureManual"));
}

int DisplayDBusProxy::colorTemperatureMode()
{
    return qvariant_cast<int>(m_dBusDisplayInter->property("ColorTemperatureManual"));
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
    return qvariant_cast<ScreenRect>(m_dBusDisplayPropertiesInter->call("Get", DisplayInterface, "PrimaryRect").arguments().at(0).value<QDBusVariant>().variant());
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
    return qvariant_cast<TouchscreenMap>(m_dBusDisplayPropertiesInter->call("Get", DisplayInterface, "TouchMap").arguments().at(0).value<QDBusVariant>().variant());
}

TouchscreenInfoList DisplayDBusProxy::touchscreens()
{
    return qvariant_cast<TouchscreenInfoList>(m_dBusDisplayPropertiesInter->call("Get", DisplayInterface, "Touchscreens").arguments().at(0).value<QDBusVariant>().variant());
}

TouchscreenInfoList_V2 DisplayDBusProxy::touchscreensV2()
{
    return qvariant_cast<TouchscreenInfoList_V2>(m_dBusDisplayPropertiesInter->call("Get", DisplayInterface, "TouchscreensV2").arguments().at(0).value<QDBusVariant>().variant());
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



void DisplayDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        if(it.key() =="Brightness") {
            emit BrightnessChanged(qdbus_cast<BrightnessMap>(changedProps.value(it.key())));
        } else if(it.key() =="PrimaryRect") {
            emit PrimaryRectChanged(qdbus_cast<ScreenRect>(changedProps.value(it.key())));
        } else if(it.key() =="TouchMap") {
            emit TouchMapChanged(qdbus_cast<TouchscreenMap>(changedProps.value(it.key())));
        } else if(it.key() =="Touchscreens") {
            emit TouchscreensChanged(qdbus_cast<TouchscreenInfoList>(changedProps.value(it.key())));
        } else if(it.key() =="TouchscreensV2") {
            emit TouchscreensV2Changed(qdbus_cast<TouchscreenInfoList_V2>(changedProps.value(it.key())));
        } else {
            QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
        }
    }
}
