// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QGuiApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLoggingCategory>
#include <QDebug>
#include <QDir>
#include <QScreen>

#include "utils.hpp"
#include <qpa/qplatformnativeinterface.h>
#include <private/qguiapplication_p.h>
#include <private/qwaylandintegration_p.h>
#include <private/qwaylandwindow_p.h>
#include "treelandworker.h"
#include "operation/personalizationworker.h"
#include "operation/model/thememodel.h"
#include "operation/personalizationexport.hpp"

#define TYPEWALLPAPER           "wallpaper"
#define TYPEGREETERBACKGROUND   "greeterbackground"
#define TYPEWINDOWOPACITY       "windowopacity"
#define TYPEWINDOWRADIUS         "windowradius"
#define TYPEICON                "icon"
#define TYPECURSOR              "cursor"
#define TYPEGTK                 "gtk"
#define TYPEFONTSIZE            "fontsize"
#define TYPEACTIVECOLOR         "activecolor"
#define TYPESTANDARDFONT        "standardfont"
#define TYPEMONOSPACEFONT       "monospacefont"

Q_LOGGING_CATEGORY(DdcPersonnalizationTreelandWorker, "dcc-personalization-treeland-woker")

TreeLandWorker::TreeLandWorker(PersonalizationModel *model, QObject *parent)
: PersonalizationWorker(model, parent)
{

}

TreeLandWorker::~TreeLandWorker()
{
#ifdef Enable_Treeland
    qDeleteAll(m_wallpaperContexts);
    m_wallpaperContexts.clear();
    
    qDeleteAll(m_wallpapers);
    m_wallpapers.clear();
    
    qDeleteAll(m_lockWallpapers);
    m_lockWallpapers.clear();
#endif
}

#ifdef Enable_Treeland

void TreeLandWorker::setWallpaperForMonitor(const QString &screen, const QString &url, bool isDark, PersonalizationExport::WallpaperSetOption option, PersonalizationExport::WallpaperType type)
{
    if (checkWallpaperLockStatus()) {
        return;
    }

    if (option == PersonalizationExport::Option_Desktop) {
        setBackgroundForMonitor(screen, url, isDark, type);
    } else if (option == PersonalizationExport::Option_Lock) {
        setLockBackForMonitor(screen, url, isDark, type);
    } else if (option == PersonalizationExport::Option_All) {
        setBackgroundForMonitor(screen, url, isDark, type);
        setLockBackForMonitor(screen, url, isDark, type);
    }
}

void TreeLandWorker::setBackgroundForMonitor(const QString &monitorName, const QString &url, bool isDark, PersonalizationExport::WallpaperType type)
{
    setWallpaper(monitorName, url, isDark, WallpaperContext::wallpaper_role_desktop, type);
}

QString TreeLandWorker::getBackgroundForMonitor(const QString &monitorName)
{
    if (m_wallpapers.contains(monitorName)) {
        return m_wallpapers.value(monitorName)->url;
    }
    return QString();
}

void TreeLandWorker::setLockBackForMonitor(const QString &monitorName, const QString &url, bool isDark, PersonalizationExport::WallpaperType type)
{
    setWallpaper(monitorName, url, isDark, WallpaperContext::wallpaper_role_lockscreen, type);
}

QString TreeLandWorker::getLockBackForMonitor(const QString &monitorName)
{
    if (m_lockWallpapers.contains(monitorName)) {
        return m_lockWallpapers.value(monitorName)->url;
    }
    return QString();
}

void TreeLandWorker::setDefault(const QJsonObject &value)
{
    const QString key = value.value("type").toString();
    const QString id = value.value("Id").toString();
    if (key == "standardfont") {
        setFontName(id);
    } else if (key == "monospacefont") {
        setMonoFontName(id);
    }
    PersonalizationWorker::setDefault(value);
}

void TreeLandWorker::setAppearanceTheme(const QString &id, bool keepAuto)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setAppearanceTheme:" << id;
    if (!keepAuto) {
        PersonalizationWorker::setAppearanceTheme(id);
    }
    if (id == ".light" && m_appearanceTheme != PersonalizationAppearanceContext::theme_type::theme_type_light) {
        m_appearanceTheme = PersonalizationAppearanceContext::theme_type::theme_type_light;
        m_appearanceContext->set_window_theme_type(PersonalizationAppearanceContext::theme_type::theme_type_light);
    } else if (id == ".dark" && m_appearanceTheme != PersonalizationAppearanceContext::theme_type::theme_type_dark) {
        m_appearanceTheme = PersonalizationAppearanceContext::theme_type::theme_type_dark;
        m_appearanceContext->set_window_theme_type(PersonalizationAppearanceContext::theme_type::theme_type_dark);
    } else if (id.isEmpty() && m_appearanceTheme != PersonalizationAppearanceContext::theme_type::theme_type_auto) {
        m_appearanceTheme = PersonalizationAppearanceContext::theme_type::theme_type_auto;
        m_appearanceContext->set_window_theme_type(PersonalizationAppearanceContext::theme_type::theme_type_auto);
    } else {
        qWarning() << "error id" << id;
    }
}

void TreeLandWorker::setFontName(const QString& fontName)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setFontName:" << fontName;
    if (m_fontName == fontName) {
        return;
    }
    m_fontName = fontName;
    m_fontContext->set_font(fontName);
}

void TreeLandWorker::setMonoFontName(const QString& monoFontName)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setMonoFontName:" << monoFontName;
    if (m_monoFontName == monoFontName) {
        return;
    }
    m_monoFontName = monoFontName;
    m_fontContext->set_monospace_font(monoFontName);
}

void TreeLandWorker::setIconTheme(const QString &id)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setIconTheme:" << id;
    if (m_iconTheme == id) {
        return;
    }
    m_iconTheme = id;
    PersonalizationWorker::setIconTheme(id);
    m_appearanceContext->set_icon_theme(id);
}

void TreeLandWorker::setCursorTheme(const QString &id)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setIconTheme:" << id;
    if (m_cursorTheme == id) {
        return;
    }
    m_cursorTheme = id;
    PersonalizationWorker::setCursorTheme(id);
    m_cursorContext->set_theme(id);
    m_cursorContext->commit();
}

void TreeLandWorker::setActiveColor(const QString &hexColor)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setActiveColor:" << hexColor;
    if (m_activeColor == hexColor) {
        return;
    }
    m_activeColor = hexColor;
    PersonalizationWorker::setActiveColor(hexColor);
    m_appearanceContext->set_active_color(hexColor);
}

void TreeLandWorker::setFontSize(const int pixelSize) 
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setFontSize:" << pixelSize;
    if (m_fontSize == pixelSize) {
        return;
    }
    m_fontSize = pixelSize;
    PersonalizationWorker::setFontSize(pixelSize);
    m_fontContext->set_font_size(pxToPt(pixelSize) * 10);
}

void TreeLandWorker::setTitleBarHeight(int value)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setTitleBarHeight:" << value;
    if (m_titleBarHeight == value) {
        return;
    }
    m_titleBarHeight = value;
    PersonalizationWorker::setTitleBarHeight(value);
    m_appearanceContext->set_window_titlebar_height(value);
}

void TreeLandWorker::setWindowRadius(int value)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setWindowRadius:" << value;
    if (m_windowRadius == value) {
        return;
    }
    m_windowRadius = value;
    PersonalizationWorker::setWindowRadius(value);
    m_appearanceContext->set_round_corner_radius(value);
}

void TreeLandWorker::setOpacity(int value)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setOpacity:" << value;
    if (m_opacity == value) {
        return;
    }
    m_opacity = value;
    PersonalizationWorker::setOpacity(value);
    m_appearanceContext->set_window_opacity(value);
}

void TreeLandWorker::setGlobalTheme(const QString &themeId)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setGlobalTheme:" << themeId;
    if (m_globalTheme == themeId) {
        return;
    }
    m_globalTheme = themeId;
    handleGlobalTheme(themeId);
    PersonalizationWorker::setGlobalTheme(themeId);
}

void TreeLandWorker::onWallpaperUrlsChanged()
{
    QVariantMap wallpaperMap;

    for (auto it = m_wallpapers.begin(); it != m_wallpapers.end(); ++it) {
        wallpaperMap.insert(it.key(), it.value()->url);
    }

    if (!wallpaperMap.isEmpty()) {
        m_model->setWallpaperMap(wallpaperMap);
    }
}

void TreeLandWorker::init()
{
    if (m_appearanceContext.isNull()) { 
        m_appearanceContext.reset(new PersonalizationAppearanceContext(m_personalizationManager->get_appearance_context(), this->m_model));
    }
    if (m_cursorContext.isNull()) {
        m_cursorContext.reset(new PersonalizationCursorContext(m_personalizationManager->get_cursor_context(), this->m_model));
    }
    if (m_fontContext.isNull()) {
        m_fontContext.reset(new PersonalizationFontContext(m_personalizationManager->get_font_context(), this->m_model));
    }
}

void TreeLandWorker::initWallpaperContext()
{
    auto screens = qApp->screens();
    for (auto screen : screens) {
        getOrCreateWallpaperContext(screen->name());
    }
}

WallpaperContext *TreeLandWorker::getOrCreateWallpaperContext(const QString &monitorName)
{
    if (m_wallpaperContexts.contains(monitorName)) {
        return m_wallpaperContexts.value(monitorName);
    }

    auto *native = QGuiApplication::platformNativeInterface();
    auto screens = qApp->screens();
    for (auto screen : screens) {
        if (screen->name() == monitorName) {
            struct wl_output *output = reinterpret_cast<wl_output *>(
                native->nativeResourceForScreen("output", screen));
            
            if (output) {
                auto *wallpaperObj = m_wallpaperManager->get_treeland_wallpaper(output, nullptr);
                if (wallpaperObj) {
                    auto *ctx = new WallpaperContext(wallpaperObj);
                    m_wallpaperContexts.insert(monitorName, ctx);
                    
                    connect(ctx, &WallpaperContext::wallpaperChanged, this, 
                        [this, monitorName](WallpaperContext::wallpaper_role role, WallpaperContext::wallpaper_source_type type, const QString &fileSource) {
                            qCDebug(DdcPersonnalizationTreelandWorker) << "Wallpaper changed for" << monitorName << "role:" << role << "source:" << fileSource;
                            
                            if (role == WallpaperContext::wallpaper_role_desktop) {
                                if (!m_wallpapers.contains(monitorName)) {
                                    m_wallpapers.insert(monitorName, new WallpaperMetaData);
                                }
                                auto *meta = m_wallpapers.value(monitorName);
                                meta->url = isURI(fileSource) ? fileSource : enCodeURI(fileSource, "file://");
                                meta->monitorName = monitorName;
                                onWallpaperUrlsChanged();
                            } else if (role == WallpaperContext::wallpaper_role_lockscreen) {
                                if (!m_lockWallpapers.contains(monitorName)) {
                                    m_lockWallpapers.insert(monitorName, new WallpaperMetaData);
                                }
                                auto *meta = m_lockWallpapers.value(monitorName);
                                meta->url = fileSource;
                                meta->monitorName = monitorName;
                            }
                        });
                    
                    return ctx;
                }
            }
            break;
        }
    }
    
    return nullptr;
}

void TreeLandWorker::setWallpaper(const QString &monitorName, const QString &url, bool isDark, uint32_t role, uint32_t type)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setWallpaper:" << monitorName << "url:" << url << "isDark:" << isDark << "role:" << role << "type:" << type;

    if (checkWallpaperLockStatus()) {
        return;
    }

    QString dest;
    if (QFile::exists(url)) {
        dest = url;
    } else {
        QUrl destUrl(url);
        dest = destUrl.toLocalFile();
    }

    if (dest.isEmpty())
        return;

    // Update wallpaper metadata
    if (!m_wallpapers.contains(monitorName)) {
        m_wallpapers.insert(monitorName, new WallpaperMetaData);
    }

    auto meta_data = m_wallpapers.value(monitorName);

    if (meta_data != nullptr) {
        meta_data->isDark = isDark;
        meta_data->url = url;
        meta_data->monitorName = monitorName;
    }

    if (m_wallpaperManager && m_wallpaperManager->isActive()) {
        qCDebug(DdcPersonnalizationTreelandWorker) << "Using new wallpaper manager protocol";

        auto *wallpaperCtx = getOrCreateWallpaperContext(monitorName);
        if (wallpaperCtx) {
            if (type == WallpaperContext::wallpaper_source_type_video) {
                qCDebug(DdcPersonnalizationTreelandWorker) << "Setting video wallpaper:" << dest;
                wallpaperCtx->setVideoSource(dest, static_cast<WallpaperContext::wallpaper_role>(role));
            } else {
                qCDebug(DdcPersonnalizationTreelandWorker) << "Setting image wallpaper:" << dest;
                wallpaperCtx->setImageSource(dest, static_cast<WallpaperContext::wallpaper_role>(role));
            }
        } else {
            qCWarning(DdcPersonnalizationTreelandWorker) << "Failed to get wallpaper context for:" << monitorName;
        }
    }
}

void TreeLandWorker::handleGlobalTheme(const QString &themeId)
{
    uint8_t mode = m_appearanceTheme;
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();

    const QMap<QString, QJsonObject> &itemList = globalTheme->getList();
    if (itemList.contains(themeId)) {
        const QString &themePath = itemList.value(themeId).value("Path").toString();
        KeyFile theme(',');
        theme.loadFile(themePath + QDir::separator() + QStringLiteral("index.theme"));
        QString defaultTheme = theme.getStr("Deepin Theme", "DefaultTheme");

        if (defaultTheme.isEmpty())
            return;
        QString darkTheme = theme.getStr("Deepin Theme", "DarkTheme");
        if (darkTheme.isEmpty())
            mode = PersonalizationAppearanceContext::theme_type_light;

        switch (mode) {
        case PersonalizationAppearanceContext::theme_type_light:
            applyGlobalTheme(theme, defaultTheme, defaultTheme, themePath);
            break;
        case PersonalizationAppearanceContext::theme_type_dark: {
            if (darkTheme.isEmpty())
                return;
            applyGlobalTheme(theme, darkTheme, defaultTheme, themePath);
            break;
        }
        case PersonalizationAppearanceContext::theme_type_auto: {
            applyGlobalTheme(theme, defaultTheme, defaultTheme, themePath);
            break;
        }
        }
    }
}

void TreeLandWorker::applyGlobalTheme(KeyFile &theme, const QString &themeName, const QString &defaultTheme, const QString &themePath)
{
    QString defTheme = (defaultTheme.isEmpty() || defaultTheme == themeName) ? QString() : defaultTheme;
    // 设置globlaTheme的一项，先从themeName中找对应项，若没有则从defTheme中找对应项，最后调用doSetByType实现功能
    auto setGlobalItem = [&theme, &themeName, &defTheme, this](const QString &key, const QString &type) {
        QString themeValue = theme.getStr(themeName, key);
        if (themeValue.isEmpty() && !defTheme.isEmpty())
            themeValue = theme.getStr(defTheme, key);
        if (!themeValue.isEmpty())
            doSetByType(type, themeValue);
    };
    auto setGlobalFile = [&theme, &themeName, &defTheme, &themePath, this](const QString &key, const QString &type) {
        QString themeValue = theme.getStr(themeName, key);
        // 如果是用户自定义的桌面壁纸, 切换主题的外观时, 不重新设置壁纸
        // if (isSkipSetWallpaper(themePath) && type == TYPEWALLPAPER) {
        //     return;
        // }
        if (themeValue.isEmpty() && !defTheme.isEmpty()) {
            themeValue = theme.getStr(defTheme, key);
        }

        if (!QFile::exists(themeValue)) {
            QString newPath = themePath + QDir::separator() + themeValue;
            bool isExist = QFile::exists(newPath);
            if (isExist) {
                themeValue = newPath;
            }
        }

        if (!themeValue.isEmpty()) {
            doSetByType(type, themeValue);
        }
    };

    // 如果是用户自定义主题, 切换外观时只单独更新外观选项
    if (themePath.endsWith("custom")) {
        return setGlobalItem("AppTheme", TYPEGTK);
    }

    setGlobalFile("Wallpaper", TYPEWALLPAPER);
    setGlobalFile("LockBackground", TYPEGREETERBACKGROUND);
    setGlobalItem("IconTheme", TYPEICON);
    setGlobalItem("CursorTheme", TYPECURSOR);
    setGlobalItem("AppTheme", TYPEGTK);
    setGlobalItem("StandardFont", TYPESTANDARDFONT);
    setGlobalItem("MonospaceFont", TYPEMONOSPACEFONT);
    setGlobalItem("FontSize", TYPEFONTSIZE);
    setGlobalItem("ActiveColor", TYPEACTIVECOLOR);
    setGlobalItem("WindowRadius", TYPEWINDOWRADIUS);
    setGlobalItem("WindowOpacity", TYPEWINDOWOPACITY);
}

void TreeLandWorker::doSetByType(const QString &type, const QString &value)
{
    if (type == TYPEWALLPAPER) {
        auto screens = qApp->screens();
        for (const auto screen : screens) {
            // FIXME(mhduiy): only set image, only set to desktop
            setWallpaper(screen->name(), value, false, WallpaperContext::wallpaper_role_desktop, WallpaperContext::wallpaper_source_type_image);
        }
    } else if(type == TYPEICON) {
        setIconTheme(value);
    } else if (type == TYPECURSOR) {
        setCursorTheme(value);
    } else if (type == TYPESTANDARDFONT) {
        setFontName(value);
    } else if (type == TYPEMONOSPACEFONT) {
        setMonoFontName(value);
    } else if (type == TYPEFONTSIZE) {
        double pointSize = value.toDouble();
        if (pointSize > 0) {
            setFontSize(ptToPx(pointSize));
        }
    } else if (type == TYPEACTIVECOLOR) {
        setActiveColor(value);
    } else if (type == TYPEWINDOWRADIUS) {
        setWindowRadius(value.toInt());
    } else if (type == TYPEWINDOWOPACITY) {
        setOpacity(value.toDouble());
    } 
}

void TreeLandWorker::active()
{
    if (m_personalizationManager.isNull()) {
        m_personalizationManager.reset(new PersonalizationManager(this));
        connect(m_personalizationManager.get(), &PersonalizationManager::activeChanged, this, [this]() {
            if (m_personalizationManager->isActive()) {
                init();
            } else {
                // clear();
            }
        });
    }

    if (m_wallpaperManager.isNull()) {
        m_wallpaperManager.reset(new WallpaperManager(this));
        connect(m_wallpaperManager.get(), &WallpaperManager::activeChanged, this, [this]() {
            if (m_wallpaperManager->isActive()) {
                initWallpaperContext();
            } else {
                // clear();
            }
        });
    }
    PersonalizationWorker::active();
}

PersonalizationManager::PersonalizationManager(QObject *parent)
    : QWaylandClientExtensionTemplate<PersonalizationManager>(1)
{
    if (QGuiApplication::platformName() == QLatin1String("wayland")) {
        QtWaylandClient::QWaylandIntegration *waylandIntegration = static_cast<QtWaylandClient::QWaylandIntegration *>(QGuiApplicationPrivate::platformIntegration());
        if (!waylandIntegration) {
            qWarning() << "waylandIntegration is nullptr!!!";
            return;
        }
        m_waylandDisplay = waylandIntegration->display();
        if (!m_waylandDisplay) {
            qWarning() << "waylandDisplay is nullptr!!!";
            return;
        }

        addListener();
    }
    setParent(parent);
}

void PersonalizationManager::addListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip addListener";
        return;
    }
    m_waylandDisplay->addRegistryListener(&handleListenerGlobal, this);
}

void PersonalizationManager::removeListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip removeListener";
        return;
    }
    m_waylandDisplay->removeListener(&handleListenerGlobal, this);
}

void PersonalizationManager::handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    if (interface == treeland_personalization_manager_v1_interface.name) {
        PersonalizationManager *integration = static_cast<PersonalizationManager *>(data);
        if (!integration) {
            qWarning() << "integration is nullptr!!!";
            return;
        }

        integration->init(registry, id, version);
    }
}

PersonalizationAppearanceContext::PersonalizationAppearanceContext(struct ::treeland_personalization_appearance_context_v1 *context, PersonalizationModel *worker)
    : QWaylandClientExtensionTemplate<PersonalizationAppearanceContext>(1)
    , QtWayland::treeland_personalization_appearance_context_v1(context)
    , m_model(worker)
{
    get_round_corner_radius();
    get_icon_theme();
    get_active_color();
    get_window_opacity();
    get_window_theme_type();
    get_window_titlebar_height();
}

void PersonalizationAppearanceContext::treeland_personalization_appearance_context_v1_round_corner_radius(int32_t radius)
{
    m_model->setWindowRadius(radius);
}

void PersonalizationAppearanceContext::treeland_personalization_appearance_context_v1_icon_theme(const QString &)
{

}

void PersonalizationAppearanceContext::treeland_personalization_appearance_context_v1_active_color(const QString &active_color)
{
    m_model->setActiveColor(active_color);
}

void PersonalizationAppearanceContext::treeland_personalization_appearance_context_v1_window_opacity(uint32_t opacity)
{
    Q_UNUSED(opacity)
    // Using the value of the appearance module, this is an invalid value
    // m_model->setOpacity(opacity / 100.0);
}

void PersonalizationAppearanceContext::treeland_personalization_appearance_context_v1_window_theme_type(uint32_t)
{
    // Using the value of the appearance module, this is an invalid value
}

void PersonalizationAppearanceContext::treeland_personalization_appearance_context_v1_window_titlebar_height(uint32_t height)
{
    m_model->setTitleBarHeight(height);
}

PersonalizationWallpaperContext::PersonalizationWallpaperContext(struct ::treeland_personalization_wallpaper_context_v1 *context)
    : QWaylandClientExtensionTemplate<PersonalizationWallpaperContext>(1)
    , QtWayland::treeland_personalization_wallpaper_context_v1(context)
{

}

void PersonalizationWallpaperContext::treeland_personalization_wallpaper_context_v1_metadata(
    const QString &metadata)
{
    Q_EMIT metadataChanged(metadata);
}

PersonalizationCursorContext::PersonalizationCursorContext(struct ::treeland_personalization_cursor_context_v1 *context, PersonalizationModel *model)
    : QWaylandClientExtensionTemplate<PersonalizationCursorContext>(1)
    , QtWayland::treeland_personalization_cursor_context_v1(context)
    , m_model(model)
{
    get_theme();
}

void PersonalizationCursorContext::treeland_personalization_cursor_context_v1_theme(const QString &)
{

}

PersonalizationFontContext::PersonalizationFontContext(struct ::treeland_personalization_font_context_v1 *context, PersonalizationModel *model)
    : QWaylandClientExtensionTemplate<PersonalizationFontContext>(1)
    , QtWayland::treeland_personalization_font_context_v1(context)
    , m_model(model)
{
    get_font();
    get_monospace_font();
    get_font_size();
}

void PersonalizationFontContext::treeland_personalization_font_context_v1_font(const QString &)
{

}

void PersonalizationFontContext::treeland_personalization_font_context_v1_monospace_font(const QString &)
{

}

void PersonalizationFontContext::treeland_personalization_font_context_v1_font_size(uint32_t)
{

}

WallpaperManager::WallpaperManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WallpaperManager>(1)
{
    if (QGuiApplication::platformName() == QLatin1String("wayland")) {
        QtWaylandClient::QWaylandIntegration *waylandIntegration = static_cast<QtWaylandClient::QWaylandIntegration *>(QGuiApplicationPrivate::platformIntegration());
        if (!waylandIntegration) {
            qWarning() << "waylandIntegration is nullptr!!!";
            return;
        }
        m_waylandDisplay = waylandIntegration->display();
        if (!m_waylandDisplay) {
            qWarning() << "waylandDisplay is nullptr!!!";
            return;
        }

        addListener();
    }
    setParent(parent);
}

void WallpaperManager::addListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip addListener";
        return;
    }
    m_waylandDisplay->addRegistryListener(&handleListenerGlobal, this);
}

void WallpaperManager::removeListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip removeListener";
        return;
    }
    m_waylandDisplay->removeListener(&handleListenerGlobal, this);
}

void WallpaperManager::handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    if (interface == treeland_wallpaper_manager_v1_interface.name) {
        WallpaperManager *manager = static_cast<WallpaperManager *>(data);
        if (!manager) {
            qWarning() << "WallpaperManager is nullptr!!!";
            return;
        }

        manager->init(registry, id, version);
    }
}

// WallpaperContext implementation
WallpaperContext::WallpaperContext(struct ::treeland_wallpaper_v1 *context)
    : QWaylandClientExtensionTemplate<WallpaperContext>(1)
    , QtWayland::treeland_wallpaper_v1(context)
{

}

void WallpaperContext::setImageSource(const QString &filePath, wallpaper_role role)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setImageSource:" << filePath << "role:" << role;
    Q_EMIT wallpaperChanged(role, WallpaperContext::wallpaper_source_type_image, filePath);
    set_image_source(filePath, static_cast<uint32_t>(role));
}

void WallpaperContext::setVideoSource(const QString &filePath, wallpaper_role role)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setVideoSource:" << filePath << "role:" << role;
    Q_EMIT wallpaperChanged(role, WallpaperContext::wallpaper_source_type_video, filePath);
    set_video_source(filePath, static_cast<uint32_t>(role));
}

void WallpaperContext::treeland_wallpaper_v1_changed(uint32_t role, uint32_t source_type, const QString &file_source)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "wallpaper changed:" << file_source << "role:" << role << "type:" << source_type;
    Q_EMIT wallpaperChanged(static_cast<wallpaper_role>(role), static_cast<wallpaper_source_type>(source_type), file_source);
}

void WallpaperContext::treeland_wallpaper_v1_failed(const QString &file_source, uint32_t error)
{
    qCWarning(DdcPersonnalizationTreelandWorker) << "wallpaper failed:" << file_source << "error:" << error;
    Q_EMIT wallpaperFailed(file_source, error);
}

#endif
