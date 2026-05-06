//SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QtWaylandClient/QWaylandClientExtension>
#include <QWaylandClientExtension>
#include <dtkgui_global.h>

#include <private/qwaylanddisplay_p.h>

#include "operation/personalizationexport.hpp"
#include "operation/personalizationmodel.h"
#include "personalizationworker.h"

#ifdef Enable_Treeland
#include "wayland-treeland-personalization-manager-v1-client-protocol.h"
#include "qwayland-treeland-personalization-manager-v1.h"
#include "wayland-treeland-wallpaper-manager-unstable-v1-client-protocol.h"
#include "qwayland-treeland-wallpaper-manager-unstable-v1.h"
#include "keyfile.h"
#endif

class PersonalizationManager;
class PersonalizationAppearanceContext;
class PersonalizationWallpaperContext;
class PersonalizationCursorContext;
class PersonalizationFontContext;
class WallpaperManager;
class WallpaperContext;

class TreeLandWorker : public PersonalizationWorker
{
Q_OBJECT
public:
    struct WallpaperMetaData
    {
        bool isDark;
        QString url;
        QString monitorName;
    };

    TreeLandWorker(PersonalizationModel *model, QObject *parent = nullptr);
    ~TreeLandWorker();

#ifdef Enable_Treeland
    void setWallpaperForMonitor(const QString &screen, const QString &url, bool isDark, PersonalizationExport::WallpaperSetOption option, PersonalizationExport::WallpaperType type) override;
    void setBackgroundForMonitor(const QString &monitorName, const QString &url, bool isDark, PersonalizationExport::WallpaperType type = PersonalizationExport::Type_Image) override;
    QString getBackgroundForMonitor(const QString &monitorName);

    void setLockBackForMonitor(const QString &monitorName, const QString &url, bool isDark, PersonalizationExport::WallpaperType type = PersonalizationExport::Type_Image) override;
    QString getLockBackForMonitor(const QString &monitorName);

    void setDefault(const QJsonObject &value) override;

    void setAppearanceTheme(const QString &id, bool keepAuto = false) override;
    uint32_t appearanceTheme() const { return m_appearanceTheme; }

    void setFontName(const QString& fontName);
    QString fontName() const { return m_fontName; }

    void setMonoFontName(const QString& monoFontName);
    QString monoFontName() const { return m_monoFontName; }

    void setIconTheme(const QString &id) override;
    QString iconTheme() const { return m_iconTheme; }

    void setCursorTheme(const QString &id) override;
    QString cursorTheme() const { return m_cursorTheme; }

    void setActiveColor(const QString &hexColor) override;
    QString activeColor() const { return m_activeColor; }

    void setFontSize(const int pixelSize) override;
    int fontSize() const { return m_fontSize; }

    void setTitleBarHeight(int value) override;
    int titleBarHeight() const { return m_titleBarHeight; }

    void setWindowRadius(int radius) override;
    int windowRadius() const { return m_windowRadius; }

    void setOpacity(const int value) override;
    int opacity() const { return m_opacity; }

    void setGlobalTheme(const QString &themeId) override;
    QString globalTheme()const { return m_globalTheme;}

    void active() override;
    void init();
    void initWallpaperContext();

public slots:
    void onWallpaperUrlsChanged() override;

signals:
    void wallpaperChanged();
    void ApppearanceThemeChanged(const QString &id);
    void IconThemeChanged(const QString &id);
    void CursorThemeChanged(const QString &id);

private:
    WallpaperContext *getOrCreateWallpaperContext(const QString &monitorName);
    void setWallpaper(const QString &monitorName, const QString &url, bool isDark, uint32_t role, uint32_t type);
    void handleGlobalTheme(const QString &themeId);
    void applyGlobalTheme(KeyFile &theme, const QString &themeName, const QString &defaultTheme, const QString &themePath);
    void doSetByType(const QString &type, const QString &value);

private:
    QScopedPointer<PersonalizationManager> m_personalizationManager;
    QScopedPointer<PersonalizationAppearanceContext> m_appearanceContext;
    QScopedPointer<PersonalizationCursorContext> m_cursorContext;
    QScopedPointer<PersonalizationFontContext> m_fontContext;
    QScopedPointer<WallpaperManager> m_wallpaperManager;
    QMap<QString, WallpaperContext *> m_wallpaperContexts;

    QMap<QString, WallpaperMetaData *> m_wallpapers;
    QMap<QString, WallpaperMetaData *> m_lockWallpapers;
    uint8_t m_appearanceTheme;
    QString m_fontName;
    QString m_monoFontName;
    QString m_iconTheme;
    QString m_cursorTheme;
    QString m_activeColor;
    int m_fontSize;
    int m_titleBarHeight;
    int m_windowRadius;
    int m_opacity;
    QString m_globalTheme;
    bool m_compactDisplay;
#endif
};

#ifdef Enable_Treeland

class PersonalizationManager: public QWaylandClientExtensionTemplate<PersonalizationManager>,
                              public QtWayland::treeland_personalization_manager_v1
{
    Q_OBJECT
public:
    explicit PersonalizationManager(QObject *parent = nullptr);

private:
    void addListener();
    void removeListener();

    static void handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version);

private:
    QtWaylandClient::QWaylandDisplay *m_waylandDisplay = nullptr;
};

class PersonalizationAppearanceContext : public QWaylandClientExtensionTemplate<PersonalizationAppearanceContext>,
                                         public QtWayland::treeland_personalization_appearance_context_v1
{
    Q_OBJECT

public:
    explicit PersonalizationAppearanceContext(struct ::treeland_personalization_appearance_context_v1 *context, PersonalizationModel *model);

protected:
    void treeland_personalization_appearance_context_v1_round_corner_radius(int32_t radius) override;
    void treeland_personalization_appearance_context_v1_icon_theme(const QString &theme_name) override;
    void treeland_personalization_appearance_context_v1_active_color(const QString &active_color) override;
    void treeland_personalization_appearance_context_v1_window_opacity(uint32_t opacity) override;
    void treeland_personalization_appearance_context_v1_window_theme_type(uint32_t type) override;
    void treeland_personalization_appearance_context_v1_window_titlebar_height(uint32_t height) override;
private:
    PersonalizationModel *m_model;
};

class PersonalizationWallpaperContext : public QWaylandClientExtensionTemplate<PersonalizationWallpaperContext>,
                                        public QtWayland::treeland_personalization_wallpaper_context_v1
{
    Q_OBJECT
public:
    explicit PersonalizationWallpaperContext(struct ::treeland_personalization_wallpaper_context_v1 *context);

Q_SIGNALS:
    void metadataChanged(const QString &meta);

protected:
    void treeland_personalization_wallpaper_context_v1_metadata(const QString &metadata) override;
};

class PersonalizationCursorContext : public QWaylandClientExtensionTemplate<PersonalizationCursorContext>,
                                     public QtWayland::treeland_personalization_cursor_context_v1
{
    Q_OBJECT
public:
    explicit PersonalizationCursorContext(struct ::treeland_personalization_cursor_context_v1 *context, PersonalizationModel *model);

protected:
    void treeland_personalization_cursor_context_v1_theme(const QString &name) override;

private:
    PersonalizationModel *m_model;
};

class PersonalizationFontContext : public QWaylandClientExtensionTemplate<PersonalizationFontContext>,
                                   public QtWayland::treeland_personalization_font_context_v1
{
    Q_OBJECT
public:
    explicit PersonalizationFontContext(struct ::treeland_personalization_font_context_v1 *context, PersonalizationModel *model);

protected:
    void treeland_personalization_font_context_v1_font(const QString &font_name) override;
    void treeland_personalization_font_context_v1_monospace_font(const QString &font_name) override;
    void treeland_personalization_font_context_v1_font_size(uint32_t font_size) override;

private:
    PersonalizationModel *m_model;
};

class WallpaperManager : public QWaylandClientExtensionTemplate<WallpaperManager>,
                         public QtWayland::treeland_wallpaper_manager_v1
{
    Q_OBJECT
public:
    explicit WallpaperManager(QObject *parent = nullptr);

private:
    void addListener();
    void removeListener();

    static void handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version);

private:
    QtWaylandClient::QWaylandDisplay *m_waylandDisplay = nullptr;
};

class WallpaperContext : public QWaylandClientExtensionTemplate<WallpaperContext>,
                         public QtWayland::treeland_wallpaper_v1
{
    Q_OBJECT
public:
    explicit WallpaperContext(struct ::treeland_wallpaper_v1 *context);

    void setImageSource(const QString &filePath, wallpaper_role role);
    void setVideoSource(const QString &filePath, wallpaper_role role);

Q_SIGNALS:
    void wallpaperChanged(wallpaper_role role, wallpaper_source_type type, const QString &fileSource);
    void wallpaperFailed(const QString &fileSource, uint32_t error);

protected:
    void treeland_wallpaper_v1_changed(uint32_t role, uint32_t source_type, const QString &file_source) override;
    void treeland_wallpaper_v1_failed(const QString &file_source, uint32_t error) override;
};
#endif