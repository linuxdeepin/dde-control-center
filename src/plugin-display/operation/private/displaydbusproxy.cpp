// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "displaydbusproxy.h"

#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingReply>

const static QString DisplayService = "org.deepin.dde.Display1";
const static QString DisplayPath = "/org/deepin/dde/Display1";
const static QString DisplayInterface = "org.deepin.dde.Display1";

const static QString AppearanceService = "org.deepin.dde.Appearance1";
const static QString AppearancePath = "/org/deepin/dde/Appearance1";
const static QString AppearanceInterface = "org.deepin.dde.Appearance1";

const static QString PowerService = "org.deepin.dde.Power1";
const static QString PowerPath = "/org/deepin/dde/Power1";
const static QString PowerInterface = "org.deepin.dde.Power1";

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
    m_dBusSystemDisplayInter = new DDBusInterface("org.deepin.dde.Display1", "/org/deepin/dde/Display1", "org.deepin.dde.Display1", QDBusConnection::systemBus(), this);
    m_dBusDisplayInter = new DDBusInterface(DisplayService, DisplayPath, DisplayInterface, QDBusConnection::sessionBus(), this);
    m_dBusAppearanceInter = new DDBusInterface(AppearanceService, AppearancePath, AppearanceInterface, QDBusConnection::sessionBus(), this);
    m_dBusPowerInter = new DDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect("com.deepin.wm", "/com/deepin/wm", "com.deepin.wm", "WorkspaceSwitched", this, SIGNAL(WorkspaceSwitched(int, int)));
}

// power
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

QString DisplayDBusProxy::wallpaperURls() const
{
    return qvariant_cast<QString>(m_dBusAppearanceInter->property("WallpaperURls"));
}

// display
BrightnessMap DisplayDBusProxy::brightness()
{
    return qvariant_cast<BrightnessMap>(m_dBusDisplayInter->property("Brightness"));
}

bool DisplayDBusProxy::colorTemperatureEnabled() const
{
    return qvariant_cast<bool>(m_dBusDisplayInter->property("ColorTemperatureEnabled"));
}

void DisplayDBusProxy::setColorTemperatureEnabled(bool enabled)
{
    m_dBusDisplayInter->setProperty("ColorTemperatureEnabled", enabled);
}

int DisplayDBusProxy::colorTemperatureManual()
{
    return qvariant_cast<int>(m_dBusDisplayInter->property("ColorTemperatureManual"));
}

int DisplayDBusProxy::colorTemperatureMode()
{
    return qvariant_cast<int>(m_dBusDisplayInter->property("ColorTemperatureMode"));
}

const QString DisplayDBusProxy::customColorTempTimePeriod()
{
    return qvariant_cast<QString>(m_dBusDisplayInter->property("CustomColorTempTimePeriod"));
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

QDBusPendingReply<QMap<QString, double>> DisplayDBusProxy::GetScreenScaleFactors()
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

QDBusPendingReply<> DisplayDBusProxy::SetScreenScaleFactors(const QMap<QString, double> &scaleFactors)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(scaleFactors);
    return m_dBusAppearanceInter->asyncCallWithArgumentList(QStringLiteral("SetScreenScaleFactors"), argumentList);
}

QDBusPendingReply<QString> DisplayDBusProxy::GetCurrentWorkspaceBackgroundForMonitor(const QString &strMonitorName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(strMonitorName);
    return m_dBusAppearanceInter->asyncCallWithArgumentList(QStringLiteral("GetCurrentWorkspaceBackgroundForMonitor"), argumentList);
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

QDBusPendingReply<> DisplayDBusProxy::SetCustomColorTempTimePeriod(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusDisplayInter->asyncCallWithArgumentList(QStringLiteral("SetCustomColorTempTimePeriod"), argumentList);
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
    return m_dBusDisplayInter->call("SupportSetColorTemperature");
}
