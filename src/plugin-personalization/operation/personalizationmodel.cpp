//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationmodel.h"
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"
#include "model/wallpapermodel.h"

PersonalizationModel::PersonalizationModel(QObject *parent)
    : QObject(parent)
{
    m_windowModel    = new ThemeModel(this);
    m_iconModel      = new ThemeModel(this);
    m_mouseModel     = new ThemeModel(this);
    m_globalThemeModel = new ThemeModel(this);
    m_standFontModel = new FontModel(this);
    m_monoFontModel  = new FontModel(this);
    m_fontSizeModel  = new FontSizeModel(this);

    m_customWallpaperSortModel = new WallpaperSortModel(this);
    m_sysWallpaperSortModel = new WallpaperSortModel(this);
    m_solidWallpaperSortModel = new WallpaperSortModel(this);
    m_screenSaverSortModel = new WallpaperSortModel(this);
    m_picScreenSaverSortModel = new WallpaperSortModel(this);

    m_customWallpaperModel = new WallpaperModel(this);
    m_sysWallpaperModel = new WallpaperModel(this);
    m_solidWallpaperModel = new WallpaperModel(this);
    m_screenSaverModel = new WallpaperModel(this);
    m_picScreenSaverModel = new WallpaperModel(this);

    m_customWallpaperSortModel->setSourceModel(m_customWallpaperModel);
    m_sysWallpaperSortModel->setSourceModel(m_sysWallpaperModel);
    m_solidWallpaperSortModel->setSourceModel(m_solidWallpaperModel);
    m_screenSaverSortModel->setSourceModel(m_screenSaverModel);
    m_picScreenSaverSortModel->setSourceModel(m_picScreenSaverModel);
    m_miniEffect = 0;
    m_currentScreenSaverPicMode = "default";
}

PersonalizationModel::~PersonalizationModel()
{

}

void PersonalizationModel::setWindowRadius(int radius)
{
    if (m_windowRadius != radius)
        m_windowRadius = radius;

    Q_EMIT  windowRadiusChanged(radius);
}

int PersonalizationModel::windowRadius()
{
    return m_windowRadius;
}

void PersonalizationModel::setOpacity(double opacity)
{
    if (m_opacity == opacity) return;

    m_opacity = opacity;

    Q_EMIT opacityChanged(opacity);
}

void PersonalizationModel::setMiniEffect(const int &effect)
{
    if(m_miniEffect == effect) return;

    m_miniEffect=effect;

    Q_EMIT miniEffectChanged(effect);
}

void PersonalizationModel::setActiveColor(const QString &color)
{
    if (m_activeColor == color)
        return;

    m_activeColor = color;

    Q_EMIT onActiveColorChanged(color);
}

void PersonalizationModel::setCompactDisplay(bool value)
{
    if (m_compactDisplay == value)
        return;
    m_compactDisplay = value;

    Q_EMIT compactDisplayChanged(value);
}


void PersonalizationModel::setScrollBarPolicy(int policy)
{
    if (m_scrollBarPolicy != policy) {
        m_scrollBarPolicy = policy;
        Q_EMIT scrollBarPolicyChanged(m_scrollBarPolicy);
    }
}

void PersonalizationModel::setTitleBarHeight(int titleBarHeight)
{
    if (m_titleBarHeight == titleBarHeight)
        return;
    m_titleBarHeight = titleBarHeight;
    Q_EMIT titleBarHeightChanged(titleBarHeight);
}

void PersonalizationModel::setIsMoveWindow(bool value)
{
    if (m_isMoveWindow != value) {
        m_isMoveWindow = value;
        Q_EMIT moveWindowChanged(value);
    }
}

void PersonalizationModel::setWindowEffectType(int windowEffectType)
{
    if (m_windowEffectType == windowEffectType)
        return;
    m_windowEffectType = windowEffectType;
    Q_EMIT  windowEffectTypeChanged(windowEffectType);
}

void PersonalizationModel::setScrollBarPolicyConfig(const QString &value)
{
    if (m_scrollBarPolicyConfig == value)
        return;
    m_scrollBarPolicyConfig = value;
    Q_EMIT scrollBarPolicyConfigChanged(value);
}

void PersonalizationModel::setCompactDisplayConfig(const QString &value)
{
    if (m_compactDisplayConfig == value)
        return;
    m_compactDisplayConfig = value;
    Q_EMIT compactDisplayConfigChanged(value);
}

void PersonalizationModel::setWallpaperMap(const QVariantMap &map)
{
    if (m_wallpaperMap == map)
        return;
    m_wallpaperMap = map;
}

void PersonalizationModel::setWallpaperSlideShowMap(const QVariantMap &map)
{
    if (m_wallpaperSlideShowMap == map)
        return;
    m_wallpaperSlideShowMap = map;
    Q_EMIT wallpaperSlideShowMapChanged(map);
}

void PersonalizationModel::setCurrentSelectScreen(const QString &screenName)
{
    if (m_currentSelectScreen == screenName)
        return;
    m_currentSelectScreen = screenName;
    Q_EMIT currentSelectScreenChanged(screenName);
}

void PersonalizationModel::setCurrentScreenSaver(const QString &currentScreenSaver)
{
    if (m_currentScreenSaver == currentScreenSaver)
        return;
    m_currentScreenSaver = currentScreenSaver;
    Q_EMIT currentScreenSaverChanged(currentScreenSaver);
}

void PersonalizationModel::setScreens(const QStringList &screens)
{
    if (m_screens == screens)
        return;
    m_screens = screens;
    if (m_currentSelectScreen.isEmpty() && !m_screens.isEmpty())
        setCurrentSelectScreen(m_screens.first());
    Q_EMIT screensChanged(screens);
}

void PersonalizationModel::setLockScreenAtAwake(bool value)
{
    if (m_lockScreenAtAwake == value)
        return;
    m_lockScreenAtAwake = value;
    Q_EMIT lockScreenAtAwakeChanged(value);
}

void PersonalizationModel::setLinePowerScreenSaverIdleTime(int value)
{
    if (m_linePowerScreenSaverIdleTime == value)
        return;

    m_linePowerScreenSaverIdleTime = value;
    qWarning() << "model: setLinePowerScreenSaverIdleTime" << value << m_linePowerScreenSaverIdleTime;
    Q_EMIT linePowerScreenSaverIdleTimeChanged(value);
}

void PersonalizationModel::setBatteryScreenSaverIdleTime(int value)
{
    if (m_batteryScreenSaverIdleTime == value)
        return;
    m_batteryScreenSaverIdleTime = value;
    Q_EMIT batteryScreenSaverIdleTimeChanged(value);
}

void PersonalizationModel::setCurrentScreenSaverPicMode(const QString &value)
{
    if (m_currentScreenSaverPicMode == value)
        return;
    m_currentScreenSaverPicMode = value;
    Q_EMIT currentScreenSaverPicModeChanged(value);
}

void PersonalizationModel::setOnBattery(bool value)
{
    if (m_onBattery == value)
        return;

    m_onBattery = value;
    Q_EMIT onBatteryChanged(value);
}
