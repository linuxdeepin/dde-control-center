//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationdbusproxy.h"

#include <DGuiApplicationHelper>
#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

const QString AppearanceService = QStringLiteral("org.deepin.dde.Appearance1");
const QString AppearancePath = QStringLiteral("/org/deepin/dde/Appearance1");
const QString AppearanceInterface = QStringLiteral("org.deepin.dde.Appearance1");

const QString WMService = QStringLiteral("com.deepin.wm");
const QString WMPath = QStringLiteral("/com/deepin/wm");
const QString WMInterface = QStringLiteral("com.deepin.wm");

const QString EffectsService = QStringLiteral("org.kde.KWin");
const QString EffectsPath = QStringLiteral("/Effects");
const QString EffectsInterface = QStringLiteral("org.kde.kwin.Effects");

const QString DaemonService = QStringLiteral("org.deepin.dde.Daemon1");
const QString DaemonPath = QStringLiteral("/org/deepin/dde/Daemon1");
const QString DaemonInterface = QStringLiteral("org.deepin.dde.Daemon1");

const QString ScreenSaverServive = QStringLiteral("com.deepin.ScreenSaver");
const QString ScreenSaverPath = QStringLiteral("/com/deepin/ScreenSaver");
const QString ScreenSaverInterface = QStringLiteral("com.deepin.ScreenSaver");

const QString WallpaperSlideshowService = QStringLiteral("org.deepin.dde.WallpaperSlideshow");
const QString WallpaperSlideshowPath = QStringLiteral("/org/deepin/dde/WallpaperSlideshow");
const QString WallpaperSlideshowInterface = QStringLiteral("org.deepin.dde.WallpaperSlideshow");

const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

DGUI_USE_NAMESPACE

PersonalizationDBusProxy::PersonalizationDBusProxy(QObject *parent)
    : QObject(parent)
{
    m_AppearanceInter = new QDBusInterface(AppearanceService, AppearancePath, AppearanceInterface, QDBusConnection::sessionBus(), this);
    m_DaemonInter = new QDBusInterface(DaemonService, DaemonPath, DaemonInterface, QDBusConnection::systemBus(), this);
    m_screenSaverInter = new QDBusInterface(ScreenSaverServive, ScreenSaverPath, ScreenSaverInterface, QDBusConnection::sessionBus(), this);
    m_wallpaperSlideshowInter = new QDBusInterface(WallpaperSlideshowService, WallpaperSlideshowPath, WallpaperSlideshowInterface, QDBusConnection::sessionBus(), this);
    if (!DGuiApplicationHelper::testAttribute(DGuiApplicationHelper::IsWaylandPlatform)) {
        m_WMInter = new QDBusInterface(WMService, WMPath, WMInterface, QDBusConnection::sessionBus(), this);
        m_EffectsInter = new QDBusInterface(EffectsService, EffectsPath, EffectsInterface, QDBusConnection::sessionBus(), this);

        QDBusConnection::sessionBus().connect(WMService, WMPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    }
    
    QDBusConnection::sessionBus().connect(AppearanceService, AppearancePath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(ScreenSaverServive, ScreenSaverPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(WallpaperSlideshowService, WallpaperSlideshowPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));

    connect(m_AppearanceInter, SIGNAL(Changed(const QString &, const QString &)), this, SIGNAL(Changed(const QString &, const QString &)));
    connect(m_AppearanceInter, SIGNAL(Refreshed(const QString &)), this, SIGNAL(Refreshed(const QString &)));
    connect(m_DaemonInter, SIGNAL(WallpaperChanged(const QString &, uint, const QStringList &)), this, SIGNAL(WallpaperChanged(const QString &, uint, const QStringList &)));
}

// Appearance
QString PersonalizationDBusProxy::background()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("Background"));
}
void PersonalizationDBusProxy::setBackground(const QString &value)
{
    m_AppearanceInter->setProperty("Background", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::cursorTheme()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("CursorTheme"));
}

void PersonalizationDBusProxy::setCursorTheme(const QString &value)
{
    m_AppearanceInter->setProperty("CursorTheme", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::globalTheme()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("GlobalTheme"));
}

void PersonalizationDBusProxy::setGlobalTheme(const QString &value)
{
    m_AppearanceInter->setProperty("GlobalTheme", QVariant::fromValue(value));
}

double PersonalizationDBusProxy::fontSize()
{
    return qvariant_cast<double>(m_AppearanceInter->property("FontSize"));
}

void PersonalizationDBusProxy::setFontSize(double value)
{
    m_AppearanceInter->setProperty("FontSize", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::gtkTheme()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("GtkTheme"));
}
void PersonalizationDBusProxy::setGtkTheme(const QString &value)
{
    m_AppearanceInter->setProperty("GtkTheme", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::iconTheme()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("IconTheme"));
}

void PersonalizationDBusProxy::setIconTheme(const QString &value)
{
    m_AppearanceInter->setProperty("IconTheme", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::monospaceFont()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("MonospaceFont"));
}

void PersonalizationDBusProxy::setMonospaceFont(const QString &value)
{
    m_AppearanceInter->setProperty("MonospaceFont", QVariant::fromValue(value));
}

double PersonalizationDBusProxy::opacity()
{
    return qvariant_cast<double>(m_AppearanceInter->property("Opacity"));
}

void PersonalizationDBusProxy::setOpacity(double value)
{
    m_AppearanceInter->setProperty("Opacity", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::qtActiveColor()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("QtActiveColor"));
}

void PersonalizationDBusProxy::setQtActiveColor(const QString &value)
{
    m_AppearanceInter->setProperty("QtActiveColor", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::standardFont()
{
    return qvariant_cast<QString>(m_AppearanceInter->property("StandardFont"));
}

void PersonalizationDBusProxy::setStandardFont(const QString &value)
{
    m_AppearanceInter->setProperty("StandardFont", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::wallpaperSlideShow()
{
    return qvariant_cast<QString>(m_wallpaperSlideshowInter->property("WallpaperSlideShow"));
}

QString PersonalizationDBusProxy::wallpaperSlideShow(const QString &monitorName)
{
    return QDBusPendingReply<QString>(m_wallpaperSlideshowInter->asyncCall(QStringLiteral("GetWallpaperSlideShow"), QVariant::fromValue(monitorName)));
}

void PersonalizationDBusProxy::setWallpaperSlideShow(const QString &monitorName, const QString &slideShow)
{
    m_wallpaperSlideshowInter->asyncCall("SetWallpaperSlideShow", QVariant::fromValue(monitorName), QVariant::fromValue(slideShow));
}

void PersonalizationDBusProxy::setWallpaperSlideShow(const QString &wallpaperSlideShow)
{
    m_wallpaperSlideshowInter->setProperty("WallpaperSlideShow", QVariant::fromValue(wallpaperSlideShow));
}

int PersonalizationDBusProxy::windowRadius()
{
    return qvariant_cast<int>(m_AppearanceInter->property("WindowRadius"));
}

void PersonalizationDBusProxy::setWindowRadius(int value)
{
    m_AppearanceInter->setProperty("WindowRadius", QVariant::fromValue(value));
}

// Appearance slot
QString PersonalizationDBusProxy::List(const QString &ty)
{
    return QDBusPendingReply<QString>(m_AppearanceInter->asyncCall(QStringLiteral("List"), QVariant::fromValue(ty)));
}

bool PersonalizationDBusProxy::List(const QString &ty, QObject *receiver, const char *member, const char *errorSlot)
{
    QList<QVariant> args;
    args << QVariant::fromValue(ty);
    return m_AppearanceInter->callWithCallback(QStringLiteral("List"), args, receiver, member, errorSlot);
}

void PersonalizationDBusProxy::Set(const QString &ty, const QString &value)
{
    m_AppearanceInter->asyncCall(QStringLiteral("Set"), QVariant::fromValue(ty), QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::Show(const QString &ty, const QStringList &names)
{
    return QDBusPendingReply<QString>(m_AppearanceInter->asyncCall(QStringLiteral("Show"), QVariant::fromValue(ty), QVariant::fromValue(names)));
}

bool PersonalizationDBusProxy::Show(const QString &ty, const QStringList &names, QObject *receiver, const char *member)
{
    QList<QVariant> args;
    args << QVariant::fromValue(ty) << QVariant::fromValue(names);
    return m_AppearanceInter->callWithCallback(QStringLiteral("Show"), args, receiver, member);
}

QString PersonalizationDBusProxy::Thumbnail(const QString &ty, const QString &name)
{
    return QDBusPendingReply<QString>(m_AppearanceInter->asyncCall(QStringLiteral("Thumbnail"), QVariant::fromValue(ty), QVariant::fromValue(name)));
}

bool PersonalizationDBusProxy::Thumbnail(const QString &ty, const QString &name, QObject *receiver, const char *member, const char *errorSlot)
{
    QList<QVariant> args;
    args << QVariant::fromValue(ty) << QVariant::fromValue(name);
    return m_AppearanceInter->callWithCallback(QStringLiteral("Thumbnail"), args, receiver, member, errorSlot);
}

int PersonalizationDBusProxy::getDTKSizeMode()
{
    return qvariant_cast<int>(m_AppearanceInter->property("DTKSizeMode"));
}

void PersonalizationDBusProxy::setDTKSizeMode(int value)
{
    m_AppearanceInter->setProperty("DTKSizeMode", QVariant::fromValue(value));
}

int PersonalizationDBusProxy::getScrollBarPolicy()
{
    return qvariant_cast<int>(m_AppearanceInter->property("QtScrollBarPolicy"));
}

void PersonalizationDBusProxy::setScrollBarPolicy(int value)
{
    m_AppearanceInter->setProperty("QtScrollBarPolicy", QVariant::fromValue(value));
}

void PersonalizationDBusProxy::SetCurrentWorkspaceBackgroundForMonitor(const QString &url, const QString &screenName)
{
    m_AppearanceInter->asyncCall(QStringLiteral("SetCurrentWorkspaceBackgroundForMonitor"), url, screenName);
}

void PersonalizationDBusProxy::SetGreeterBackground(const QString &url)
{
    m_AppearanceInter->asyncCall(QStringLiteral("Set"), QStringLiteral("greeterbackground"), QVariant::fromValue(url));
}

QString PersonalizationDBusProxy::getCurrentWorkSpaceBackgroundForMonitor(const QString &screenName)
{
    return QDBusPendingReply<QString>(m_AppearanceInter->asyncCall(QStringLiteral("GetCurrentWorkspaceBackgroundForMonitor"), screenName));
}

// Daemon
void PersonalizationDBusProxy::deleteCustomWallpaper(const QString &userName, const QString &url)
{
    m_DaemonInter->asyncCall(QStringLiteral("DeleteCustomWallPaper"), QVariant::fromValue(userName), QVariant::fromValue(url));
}

QString PersonalizationDBusProxy::saveCustomWallpaper(const QString &userName, const QString &url)
{
    return QDBusPendingReply<QString>(m_DaemonInter->asyncCall(QStringLiteral("SaveCustomWallPaper"), QVariant::fromValue(userName), QVariant::fromValue(url)));
}

QStringList PersonalizationDBusProxy::getCustomWallpaper(const QString &userName)
{
    return QDBusPendingReply<QStringList>(m_DaemonInter->asyncCall(QStringLiteral("GetCustomWallPapers"), QVariant::fromValue(userName)));
}

// screenSaver
QStringList PersonalizationDBusProxy::getAllscreensaver()
{
    return qvariant_cast<QStringList>(m_screenSaverInter->property("allScreenSaver"));
}

QString PersonalizationDBusProxy::GetScreenSaverCover(const QString &name)
{
    return QDBusPendingReply<QString>(m_screenSaverInter->asyncCall(QStringLiteral("GetScreenSaverCover"), QVariant::fromValue(name)));
}

QStringList PersonalizationDBusProxy::ConfigurableItems()
{
    return QDBusPendingReply<QStringList>(m_screenSaverInter->asyncCall(QStringLiteral("ConfigurableItems")));
}

void PersonalizationDBusProxy::setCurrentScreenSaver(const QString &value)
{
    m_screenSaverInter->setProperty("currentScreenSaver", QVariant::fromValue(value));
}

QString PersonalizationDBusProxy::getCurrentScreenSaver()
{
    return qvariant_cast<QString>(m_screenSaverInter->property("currentScreenSaver"));
}

void PersonalizationDBusProxy::startScreenSaver()
{
    m_screenSaverInter->asyncCall(QStringLiteral("Start"));
}

void PersonalizationDBusProxy::stopScreenSaver()
{
    m_screenSaverInter->asyncCall(QStringLiteral("Stop"));
}

void PersonalizationDBusProxy::setLinePowerScreenSaverTimeout(int value)
{
    m_screenSaverInter->setProperty("linePowerScreenSaverTimeout", QVariant::fromValue(value));
}

void PersonalizationDBusProxy::setBatteryScreenSaverTimeout(int value)
{
    m_screenSaverInter->setProperty("batteryScreenSaverTimeout", QVariant::fromValue(value));
}

int PersonalizationDBusProxy::getLinePowerScreenSaverTimeout()
{
    return qvariant_cast<int>(m_screenSaverInter->property("linePowerScreenSaverTimeout"));
}

int PersonalizationDBusProxy::getBatteryScreenSaverTimeout()
{
    return qvariant_cast<int>(m_screenSaverInter->property("batteryScreenSaverTimeout"));
}

void PersonalizationDBusProxy::requestScreenSaverConfig(const QString& name)
{
    m_screenSaverInter->asyncCall(QStringLiteral("StartCustomConfig"), QVariant::fromValue(name));
}

bool PersonalizationDBusProxy::getLockScreenAtAwake()
{
    return qvariant_cast<bool>(m_screenSaverInter->property("lockScreenAtAwake"));
}

void PersonalizationDBusProxy::setLockScreenAtAwake(bool value)
{
    m_screenSaverInter->setProperty("lockScreenAtAwake", QVariant::fromValue(value));
}

// WM
bool PersonalizationDBusProxy::compositingAllowSwitch()
{
    if (!m_WMInter) {
        return false;
    }
    return qvariant_cast<bool>(m_WMInter->property("compositingAllowSwitch"));
}

bool PersonalizationDBusProxy::compositingEnabled()
{
    if (!m_WMInter) {
        return false;
    }
    return qvariant_cast<bool>(m_WMInter->property("compositingEnabled"));
}

void PersonalizationDBusProxy::setCompositingEnabled(bool value)
{
    if (!m_WMInter) {
        return;
    }
    m_WMInter->setProperty("compositingEnabled", QVariant::fromValue(value));
}

bool PersonalizationDBusProxy::compositingPossible()
{
    if (!m_WMInter) {
        return false;
    }
    return qvariant_cast<bool>(m_WMInter->property("compositingPossible"));
}

int PersonalizationDBusProxy::cursorSize()
{
    if (!m_WMInter) {
        return 0;
    }
    return qvariant_cast<int>(m_WMInter->property("cursorSize"));
}

void PersonalizationDBusProxy::setCursorSize(int value)
{
    if (!m_WMInter) {
        return;
    }
    m_WMInter->setProperty("cursorSize", QVariant::fromValue(value));
}

bool PersonalizationDBusProxy::zoneEnabled()
{
    if (!m_WMInter) {
        return false;
    }
    return qvariant_cast<bool>(m_WMInter->property("zoneEnabled"));
}

void PersonalizationDBusProxy::setZoneEnabled(bool value)
{
    if (!m_WMInter) {
        return;
    }
    m_WMInter->setProperty("zoneEnabled", QVariant::fromValue(value));
}

// Effects
bool PersonalizationDBusProxy::loadEffect(const QString &name)
{
    if (!m_WMInter) {
        return false;
    }
    return QDBusPendingReply<bool>(m_EffectsInter->asyncCall(QStringLiteral("loadEffect"), QVariant::fromValue(name)));
}

void PersonalizationDBusProxy::unloadEffect(const QString &name)
{
    if (!m_EffectsInter) {
        return;
    }
    m_EffectsInter->asyncCall(QStringLiteral("unloadEffect"), QVariant::fromValue(name));
}

bool PersonalizationDBusProxy::isEffectLoaded(const QString &name)
{
    if (!m_WMInter) {
        return false;
    }
    return QDBusPendingReply<bool>(m_EffectsInter->asyncCall(QStringLiteral("isEffectLoaded"), QVariant::fromValue(name)));
}

bool PersonalizationDBusProxy::isEffectLoaded(const QString &name, QObject *receiver, const char *member)
{
    if (!m_WMInter) {
        return false;
    }
    QList<QVariant> args;
    args << QVariant::fromValue(name);
    return m_EffectsInter->callWithCallback(QStringLiteral("isEffectLoaded"), args, receiver, member);
}

QString PersonalizationDBusProxy::activeColors()
{
    return QDBusPendingReply<QString>(m_AppearanceInter->asyncCall(QStringLiteral("GetActiveColors")));
}

void PersonalizationDBusProxy::setActiveColors(const QString &activeColors)
{
    m_AppearanceInter->asyncCall(QStringLiteral("SetActiveColors"), QVariant::fromValue(activeColors));
}

void PersonalizationDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.cbegin(); it != changedProps.cend(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
