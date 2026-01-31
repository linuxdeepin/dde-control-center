//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PERSONALIZATIONMODEL_H
#define PERSONALIZATIONMODEL_H

#include <QObject>
#include <QDebug>

class ThemeModel;
class FontModel;
class FontSizeModel;
class WallpaperModel;
class WallpaperSortModel;
class PersonalizationModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;

    Q_PROPERTY(int miniEffect READ miniEffect WRITE setMiniEffect NOTIFY miniEffectChanged)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(int windowRadius READ windowRadius WRITE setWindowRadius NOTIFY windowRadiusChanged)
    Q_PROPERTY(bool compactDisplay READ getCompactDisplay WRITE setCompactDisplay NOTIFY compactDisplayChanged)
    Q_PROPERTY(int scrollBarPolicy READ getScrollBarPolicy WRITE setScrollBarPolicy NOTIFY scrollBarPolicyChanged)
    Q_PROPERTY(int titleBarHeight READ getTitleBarHeight WRITE setTitleBarHeight NOTIFY titleBarHeightChanged)
    Q_PROPERTY(bool isMoveWindow READ getIsMoveWindow WRITE setIsMoveWindow NOTIFY moveWindowChanged)
    Q_PROPERTY(int windowEffectType READ windowEffectType WRITE setWindowEffectType NOTIFY windowEffectTypeChanged)
    Q_PROPERTY(QString activeColor READ getActiveColor WRITE setActiveColor NOTIFY onActiveColorChanged)
    Q_PROPERTY(QString scrollBarPolicyConfig READ getScrollBarPolicyConfig WRITE setScrollBarPolicyConfig NOTIFY scrollBarPolicyConfigChanged)
    Q_PROPERTY(QString compactDisplayConfig READ getCompactDisplayConfig WRITE setCompactDisplayConfig NOTIFY compactDisplayConfigChanged)
    Q_PROPERTY(QVariantMap wallpaperMap READ getWallpaperMap WRITE setWallpaperMap NOTIFY wallpaperMapChanged)
    Q_PROPERTY(QString currentScreenSaver READ getCurrentScreenSaver WRITE setCurrentScreenSaver NOTIFY currentScreenSaverChanged)
    Q_PROPERTY(QString currentSelectScreen READ getCurrentSelectScreen WRITE setCurrentSelectScreen NOTIFY currentSelectScreenChanged)
    Q_PROPERTY(QStringList screens READ getScreens WRITE setScreens NOTIFY screensChanged)
    Q_PROPERTY(bool lockScreenAtAwake WRITE setLockScreenAtAwake READ getLockScreenAtAwake NOTIFY lockScreenAtAwakeChanged)
    Q_PROPERTY(int linePowerScreenSaverIdleTime WRITE setLinePowerScreenSaverIdleTime READ getLinePowerScreenSaverIdleTime NOTIFY linePowerScreenSaverIdleTimeChanged)
    Q_PROPERTY(int batteryScreenSaverIdleTime WRITE setBatteryScreenSaverIdleTime READ getBatteryScreenSaverIdleTime NOTIFY batteryScreenSaverIdleTimeChanged)
    Q_PROPERTY(QString currentScreenSaverPicMode WRITE setCurrentScreenSaverPicMode READ getCurrentScreenSaverPicMode NOTIFY currentScreenSaverPicModeChanged)
    Q_PROPERTY(QVariantMap wallpaperSlideShowMap WRITE setWallpaperSlideShowMap READ getWallpaperSlideShowMap NOTIFY wallpaperSlideShowMapChanged)
    Q_PROPERTY(bool onBattery READ getOnBattery WRITE setOnBattery NOTIFY onBatteryChanged)
    Q_PROPERTY(QStringList supportEffects READ supportEffects WRITE setSupportEffects NOTIFY supportEffectsChanged)

    Q_PROPERTY(FontSizeModel *fontSizeModel MEMBER m_fontSizeModel CONSTANT)
    Q_PROPERTY(FontModel *standardFontModel MEMBER m_standFontModel CONSTANT)
    Q_PROPERTY(FontModel *monoFontModel MEMBER m_monoFontModel CONSTANT)

    Q_PROPERTY(ThemeModel *iconModel MEMBER m_iconModel CONSTANT)
    Q_PROPERTY(ThemeModel *cursorModel MEMBER m_mouseModel CONSTANT)

    Q_PROPERTY(WallpaperSortModel *customWallpaperModel MEMBER m_customWallpaperSortModel CONSTANT)
    Q_PROPERTY(WallpaperSortModel *sysWallpaperModel MEMBER m_sysWallpaperSortModel CONSTANT)
    Q_PROPERTY(WallpaperSortModel *solidWallpaperModel MEMBER m_solidWallpaperSortModel CONSTANT)

    Q_PROPERTY(WallpaperSortModel *screenSaverModel MEMBER m_screenSaverSortModel CONSTANT)
    Q_PROPERTY(WallpaperSortModel *picScreenSaverModel MEMBER m_picScreenSaverSortModel CONSTANT)

public:
    explicit PersonalizationModel(QObject *parent = nullptr);
    ~PersonalizationModel();
    inline ThemeModel *getWindowModel() const { return m_windowModel; }
    inline ThemeModel *getIconModel() const { return m_iconModel; }
    inline ThemeModel *getMouseModel() const { return m_mouseModel; }
    inline ThemeModel *getGlobalThemeModel() const { return m_globalThemeModel; }
    inline FontModel *getStandFontModel() const { return m_standFontModel; }
    inline FontModel *getMonoFontModel() const { return m_monoFontModel; }
    inline FontSizeModel *getFontSizeModel() const { return m_fontSizeModel; }

    inline WallpaperModel *getCustomWallpaperModel() const { return m_customWallpaperModel; }
    inline WallpaperModel *getSysWallpaperModel() const { return m_sysWallpaperModel; }
    inline WallpaperModel *getSolidWallpaperModel() const { return m_solidWallpaperModel; }
    inline WallpaperModel *getScreenSaverModel() const { return m_screenSaverModel; }
    inline WallpaperModel *getPicScreenSaverModel() const { return m_picScreenSaverModel; }

    void setWindowRadius(int radius);
    int windowRadius();

    void setWindowEffectType(int windowEffectType);
    inline int windowEffectType() const { return m_windowEffectType; }

    inline double opacity() const { return m_opacity; }
    void setOpacity(double opacity);

    inline int miniEffect() const { return m_miniEffect; }
    void setMiniEffect(const int &effect);

    inline QString getActiveColor() { return m_activeColor; }
    void setActiveColor(const QString &color);

    inline bool getCompactDisplay() { return m_compactDisplay; }
    void setCompactDisplay(bool value);

    inline int getScrollBarPolicy() const { return m_scrollBarPolicy;}
    void setScrollBarPolicy(int policy);

    inline int getTitleBarHeight() const { return m_titleBarHeight; }
    void setTitleBarHeight(int titleBarHeight);

    void setIsMoveWindow(const bool isMoveWindow);
    bool getIsMoveWindow() const { return m_isMoveWindow; };

    inline QString getScrollBarPolicyConfig() const { return m_scrollBarPolicyConfig; }
    void setScrollBarPolicyConfig(const QString &config);

    inline QString getCompactDisplayConfig() const { return m_compactDisplayConfig; }
    void setCompactDisplayConfig(const QString &config);

    inline QVariantMap getWallpaperMap() const { return m_wallpaperMap; }
    void setWallpaperMap(const QVariantMap &map);

    inline QVariantMap getWallpaperSlideShowMap() const { return m_wallpaperSlideShowMap; }
    void setWallpaperSlideShowMap(const QVariantMap &map);

    inline QString getCurrentSelectScreen() const { return m_currentSelectScreen; }
    void setCurrentSelectScreen(const QString &screen);

    inline QString getCurrentScreenSaver() const { return m_currentScreenSaver; }
    void setCurrentScreenSaver(const QString &currentScreenSaver);

    inline QStringList getScreens() const { return m_screens; }
    void setScreens(const QStringList &screens);

    inline bool getLockScreenAtAwake() const { return m_lockScreenAtAwake; }
    void setLockScreenAtAwake(bool value);

    inline int getLinePowerScreenSaverIdleTime() const { return m_linePowerScreenSaverIdleTime; };
    void setLinePowerScreenSaverIdleTime(int value);

    inline int getBatteryScreenSaverIdleTime() const { return m_batteryScreenSaverIdleTime; };
    void setBatteryScreenSaverIdleTime(int value);

    inline QString getCurrentScreenSaverPicMode() const { return m_currentScreenSaverPicMode; }
    void setCurrentScreenSaverPicMode(const QString &mode);

    inline bool getOnBattery() const { return m_onBattery; }
    void setOnBattery(bool value);

    inline QStringList supportEffects() const { return m_supportEffects; }
    void setSupportEffects(const QStringList &value);

Q_SIGNALS:
    void wmChanged(const bool is3d);
    void opacityChanged(double opacity);
    void miniEffectChanged(int effect);
    void onActiveColorChanged(const QString &color);
    void onCompositingAllowSwitch(bool value);
    void windowRadiusChanged(int radius);
    void onSaveWindowRadiusChanged(int radius);
    void compactDisplayChanged(bool value);
    void scrollBarPolicyChanged(int policy);
    void titleBarHeightChanged(int titleBarHeight);
    void moveWindowChanged(const bool isMoveWindow);
    void windowEffectTypeChanged(int windowEffectType);
    void scrollBarPolicyConfigChanged(const QString &config);
    void compactDisplayConfigChanged(const QString &config);
    void wallpaperMapChanged(const QVariantMap &map);
    void currentSelectScreenChanged(const QString &screen);
    void currentScreenSaverChanged(const QString &current);
    void screensChanged(const QStringList &screens);
    void lockScreenAtAwakeChanged(bool value);
    void linePowerScreenSaverIdleTimeChanged(int value);
    void batteryScreenSaverIdleTimeChanged(int value);
    void currentScreenSaverPicModeChanged(const QString &value);
    void wallpaperSlideShowMapChanged(const QVariantMap &value);
    void onBatteryChanged(bool value);
    void miniEffectChanged(bool value);
    void supportEffectsChanged(const QStringList &value);
private:
    ThemeModel *m_windowModel;
    ThemeModel *m_iconModel;
    ThemeModel *m_mouseModel;
    ThemeModel *m_globalThemeModel;
    FontModel *m_standFontModel;
    FontModel *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;

    WallpaperSortModel *m_customWallpaperSortModel;
    WallpaperSortModel *m_sysWallpaperSortModel;
    WallpaperSortModel *m_solidWallpaperSortModel;
    WallpaperSortModel *m_screenSaverSortModel;
    WallpaperSortModel *m_picScreenSaverSortModel;

    WallpaperModel *m_customWallpaperModel;
    WallpaperModel *m_sysWallpaperModel;
    WallpaperModel *m_solidWallpaperModel;
    WallpaperModel *m_screenSaverModel;
    WallpaperModel *m_picScreenSaverModel;

    bool m_isMoveWindow;
    double m_opacity;
    int m_miniEffect;
    QString m_activeColor;
    int m_windowRadius;
    bool m_compactDisplay;
    int m_scrollBarPolicy;
    int m_titleBarHeight;
    int m_windowEffectType;

    QString m_scrollBarPolicyConfig;
    QString m_compactDisplayConfig;

    QVariantMap m_wallpaperMap;
    QVariantMap m_wallpaperSlideShowMap;
    QString m_currentSelectScreen;
    QString m_currentScreenSaver;
    QStringList m_screens;
    bool m_lockScreenAtAwake;
    int m_linePowerScreenSaverIdleTime;
    int m_batteryScreenSaverIdleTime;
    QString m_currentScreenSaverPicMode;
    bool m_onBattery;
    QStringList m_supportEffects;
};
#endif // PERSONALIZATIONMODEL_H
