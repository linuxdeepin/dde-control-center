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
    , m_allowSwitch(false)
{
    m_windowModel    = new ThemeModel(this);
    m_iconModel      = new ThemeModel(this);
    m_mouseModel     = new ThemeModel(this);
    m_globalThemeModel = new ThemeModel(this);
    m_standFontModel = new FontModel(this);
    m_monoFontModel  = new FontModel(this);
    m_fontSizeModel  = new FontSizeModel(this);
    m_wallpaperModel = new WallpaperModel(this);
    m_is3DWm = true;
    m_miniEffect = 0;
}

PersonalizationModel::~PersonalizationModel()
{

}

void PersonalizationModel::setIs3DWm(const bool is3d)
{
    if (is3d != m_is3DWm) {
        m_is3DWm = is3d;
        Q_EMIT wmChanged(is3d);
    }
}

bool PersonalizationModel::is3DWm() const
{
    return m_is3DWm;
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

void PersonalizationModel::setCompositingAllowSwitch(bool value)
{
    if (m_allowSwitch == value)
        return;
    m_allowSwitch = value;

    Q_EMIT onCompositingAllowSwitch(value);
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

void PersonalizationModel::setTitleBarDefaultHeight(int titleBarDefaultHeight)
{
    m_titleBarDefaultHeight = titleBarDefaultHeight;
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
    Q_EMIT wallpaperMapChanged(map);
}

void PersonalizationModel::setCurrentSelectScreen(const QString &screenName)
{
    if (m_currentSelectScreen == screenName)
        return;
    m_currentSelectScreen = screenName;
    Q_EMIT currentSelectScreenChanged(screenName);
}

void PersonalizationModel::setScreens(const QStringList &screens)
{
    if (m_screens == screens)
        return;
    m_screens = screens;
    Q_EMIT screensChanged(screens);
}
