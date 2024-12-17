//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
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
    Q_PROPERTY(QString currentSelectScreen READ getCurrentSelectScreen WRITE setCurrentSelectScreen NOTIFY currentSelectScreenChanged)
    Q_PROPERTY(QStringList screens READ getScreens WRITE setScreens NOTIFY screensChanged)

    Q_PROPERTY(FontSizeModel *fontSizeModel MEMBER m_fontSizeModel CONSTANT)
    Q_PROPERTY(FontModel *standardFontModel MEMBER m_standFontModel CONSTANT)
    Q_PROPERTY(FontModel *monoFontModel MEMBER m_monoFontModel CONSTANT)
    Q_PROPERTY(ThemeModel *iconModel MEMBER m_iconModel CONSTANT)
    Q_PROPERTY(ThemeModel *cursorModel MEMBER m_mouseModel CONSTANT)
    Q_PROPERTY(WallpaperModel *wallpaperModel MEMBER m_wallpaperModel CONSTANT)

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
    inline WallpaperModel *getWallpaperModel() const { return m_wallpaperModel; }

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

    inline QString getCurrentSelectScreen() const { return m_currentSelectScreen; }
    void setCurrentSelectScreen(const QString &screen);

    inline QStringList getScreens() const { return m_screens; }
    void setScreens(const QStringList &screens);

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
    void screensChanged(const QStringList &screens);

private:
    ThemeModel *m_windowModel;
    ThemeModel *m_iconModel;
    ThemeModel *m_mouseModel;
    ThemeModel *m_globalThemeModel;
    FontModel *m_standFontModel;
    FontModel *m_monoFontModel;
    FontSizeModel *m_fontSizeModel;
    WallpaperModel *m_wallpaperModel;

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
    QString m_currentSelectScreen;
    QStringList m_screens;
};
#endif // PERSONALIZATIONMODEL_H
