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
#include <QColor>
#include <DConfig>

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

namespace {
constexpr char DccAppId[] = "org.deepin.dde.control-center";
constexpr char PersonalizationConfig[] = "org.deepin.dde.control-center.personalization";
constexpr char TreelandDesktopWallpaperCustomizedKey[] = "treelandDesktopWallpaperCustomized";

QString globalThemeBaseId(QString themeId)
{
    if (themeId.endsWith(".light")) {
        themeId.chop(6);
    } else if (themeId.endsWith(".dark")) {
        themeId.chop(5);
    }
    return themeId;
}
}

TreeLandWorker::TreeLandWorker(PersonalizationModel *model, QObject *parent)
: PersonalizationWorker(model, parent)
{
#ifdef Enable_Treeland
    initDConfig();
#endif
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

void TreeLandWorker::initDConfig()
{
    m_dconfig = Dtk::Core::DConfig::create(DccAppId, PersonalizationConfig, QString(), this);
    if (!m_dconfig || !m_dconfig->isValid()) {
        qCWarning(DdcPersonnalizationTreelandWorker) << "Failed to create personalization DConfig";
        return;
    }

    m_desktopWallpaperCustomized = m_dconfig->value(TreelandDesktopWallpaperCustomizedKey, false).toBool();
}

void TreeLandWorker::setDesktopWallpaperCustomized(bool customized)
{
    if (m_desktopWallpaperCustomized == customized) {
        return;
    }

    m_desktopWallpaperCustomized = customized;
    if (m_dconfig && m_dconfig->isValid()) {
        m_dconfig->setValue(TreelandDesktopWallpaperCustomizedKey, customized);
    }
}

void TreeLandWorker::setWallpaperForMonitor(const QString &screen, const QString &url, PersonalizationExport::WallpaperSetOption option, PersonalizationExport::WallpaperSetType type)
{
    if (checkWallpaperLockStatus()) {
        return;
    }

    if (option == PersonalizationExport::Option_Desktop) {
        setBackgroundForMonitor(screen, url, type);
    } else if (option == PersonalizationExport::Option_Lock) {
        setLockBackForMonitor(screen, url, type);
    } else if (option == PersonalizationExport::Option_All) {
        setBackgroundForMonitor(screen, url, type);
        setLockBackForMonitor(screen, url, type);
    }
}

void TreeLandWorker::setBackgroundForMonitor(const QString &monitorName, const QString &url, PersonalizationExport::WallpaperSetType type)
{
    m_wallpaperWorker->setWantToSetWallpaper(nullptr);
    if (setWallpaper(monitorName, url, WallpaperContext::wallpaper_role_desktop, type)) {
        setDesktopWallpaperCustomized(true);
    }
}

QString TreeLandWorker::getBackgroundForMonitor(const QString &monitorName)
{
    if (m_wallpapers.contains(monitorName)) {
        return m_wallpapers.value(monitorName)->url;
    }
    return QString();
}

void TreeLandWorker::setLockBackForMonitor(const QString &monitorName, const QString &url, PersonalizationExport::WallpaperSetType type)
{
    setWallpaper(monitorName, url, WallpaperContext::wallpaper_role_lockscreen, type);
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
    if (id == ".light" && m_appearanceTheme != 0) {
        m_appearanceTheme = 0;
        m_appearanceManager->set_color_scheme(0);
    } else if (id == ".dark" && m_appearanceTheme != 1) {
        m_appearanceTheme = 1;
        m_appearanceManager->set_color_scheme(1);
    } else if (id.isEmpty() && m_appearanceTheme != 0) {
        m_appearanceTheme = 0;
        m_appearanceManager->set_color_scheme(0);
    } else {
        qWarning() << "error id" << id << m_appearanceTheme;
        return;
    }

    if (!keepAuto) {
        handleGlobalTheme(m_globalTheme);
    }
}

void TreeLandWorker::setFontName(const QString& fontName)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setFontName:" << fontName;
    if (m_fontName == fontName) {
        return;
    }
    m_fontName = fontName;
    m_appearanceManager->set_font(fontName);
}

void TreeLandWorker::setMonoFontName(const QString& monoFontName)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setMonoFontName:" << monoFontName;
    if (m_monoFontName == monoFontName) {
        return;
    }
    m_monoFontName = monoFontName;
    m_appearanceManager->set_monospace_font(monoFontName);
}

void TreeLandWorker::setIconTheme(const QString &id)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setIconTheme:" << id;
    if (m_iconTheme == id) {
        return;
    }
    m_iconTheme = id;
    PersonalizationWorker::setIconTheme(id);
    m_appearanceManager->set_icon_theme(id);
}

void TreeLandWorker::setCursorTheme(const QString &id)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setIconTheme:" << id;
    if (m_cursorTheme == id) {
        return;
    }
    m_cursorTheme = id;
    PersonalizationWorker::setCursorTheme(id);
    m_appearanceManager->set_cursor_theme(id);
}

void TreeLandWorker::setActiveColor(const QString &hexColor)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setActiveColor:" << hexColor;
    if (m_activeColor == hexColor) {
        return;
    }
    m_activeColor = hexColor;
    PersonalizationWorker::setActiveColor(hexColor);
    QColor color(hexColor);
    m_appearanceManager->set_accent_color(color.red(), color.green(), color.blue());
}

void TreeLandWorker::setFontSize(const int pixelSize) 
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setFontSize:" << pixelSize;
    if (m_fontSize == pixelSize) {
        return;
    }
    m_fontSize = pixelSize;
    PersonalizationWorker::setFontSize(pixelSize);
    m_appearanceManager->set_font_size(pxToPt(pixelSize) * 10);
}

void TreeLandWorker::setTitleBarHeight(int value)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setTitleBarHeight:" << value;
    if (m_titleBarHeight == value) {
        return;
    }
    m_titleBarHeight = value;
    PersonalizationWorker::setTitleBarHeight(value);
    m_appearanceManager->set_titlebar_height(value);
}

void TreeLandWorker::setWindowRadius(int value)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setWindowRadius:" << value;
    if (m_windowRadius == value) {
        return;
    }
    m_windowRadius = value;
    PersonalizationWorker::setWindowRadius(value);
    m_appearanceManager->set_corner_radius(value);
}

void TreeLandWorker::setOpacity(int value)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setOpacity:" << value;
    if (m_opacity == value) {
        return;
    }
    m_opacity = value;
    PersonalizationWorker::setOpacity(value);
    m_appearanceManager->set_window_opacity(wl_fixed_from_double(value / 100.0));
}

void TreeLandWorker::setGlobalTheme(const QString &themeId)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setGlobalTheme:" << themeId;
    if (m_globalTheme == themeId) {
        return;
    }

    bool globalThemeChanged = globalThemeBaseId(themeId) != globalThemeBaseId(m_globalTheme);
    if (globalThemeChanged) {
        setDesktopWallpaperCustomized(false);
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

void TreeLandWorker::onWindowThemeTypeChanged(uint32_t type)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "Window theme type changed:" << type;
    m_appearanceTheme = static_cast<uint8_t>(type);
}

void TreeLandWorker::init()
{
    if (m_appearance.isNull()) {
        m_appearance.reset(new TreelandAppearance(this->m_model));
    }

    connect(m_appearance.get(), &TreelandAppearance::colorSchemeChanged, this, &TreeLandWorker::onWindowThemeTypeChanged);
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
                        [this, monitorName](WallpaperContext::wallpaper_role role, WallpaperContext::wallpaper_source_type, const QString &fileSource) {
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

bool TreeLandWorker::setWallpaper(const QString &monitorName, const QString &url, uint32_t role, uint32_t type)
{
    qCDebug(DdcPersonnalizationTreelandWorker) << "setWallpaper:" << monitorName << "url:" << url << "role:" << role << "type:" << type;

    if (checkWallpaperLockStatus()) {
        return false;
    }

    QString dest;
    if (QFile::exists(url)) {
        dest = url;
    } else {
        QUrl destUrl(url);
        dest = destUrl.toLocalFile();
    }

    if (dest.isEmpty())
        return false;

    auto updateWallpaperMetaData = [monitorName, url](QMap<QString, WallpaperMetaData *> &wallpapers) {
        if (!wallpapers.contains(monitorName)) {
            wallpapers.insert(monitorName, new WallpaperMetaData);
        }

        auto *metaData = wallpapers.value(monitorName);
        if (metaData) {
            metaData->url = url;
            metaData->monitorName = monitorName;
        }
    };

    if (role == WallpaperContext::wallpaper_role_desktop) {
        updateWallpaperMetaData(m_wallpapers);
    } else if (role == WallpaperContext::wallpaper_role_lockscreen) {
        updateWallpaperMetaData(m_lockWallpapers);
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
            return false;
        }
    }

    return true;
}

void TreeLandWorker::handleGlobalTheme(const QString &themeId)
{
    uint8_t mode = m_appearanceTheme;
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();
    const QString &baseThemeId = globalThemeBaseId(themeId);

    const QMap<QString, QJsonObject> &itemList = globalTheme->getList();
    if (itemList.contains(baseThemeId)) {
        const QString &themePath = itemList.value(baseThemeId).value("Path").toString();
        KeyFile theme(',');
        theme.loadFile(themePath + QDir::separator() + QStringLiteral("index.theme"));
        QString defaultTheme = theme.getStr("Deepin Theme", "DefaultTheme");

        if (defaultTheme.isEmpty())
            return;
        QString darkTheme = theme.getStr("Deepin Theme", "DarkTheme");
        if (darkTheme.isEmpty())
            mode = 0;

        switch (mode) {
        case 0:
            applyGlobalTheme(theme, defaultTheme, defaultTheme, themePath);
            break;
        case 1: {
            if (darkTheme.isEmpty())
                return;
            applyGlobalTheme(theme, darkTheme, defaultTheme, themePath);
            break;
        }
        default:
            qWarning() << "theme mode invalid: " << mode;
        }
    }
}

void TreeLandWorker::applyGlobalTheme(KeyFile &theme, const QString &themeName, const QString &defaultTheme, const QString &themePath)
{
    QString defTheme = (defaultTheme.isEmpty() || defaultTheme == themeName) ? QString() : defaultTheme;
    auto setGlobalItem = [&theme, &themeName, &defTheme, this](const QString &key, const QString &type) {
        QString themeValue = theme.getStr(themeName, key);
        if (themeValue.isEmpty() && !defTheme.isEmpty())
            themeValue = theme.getStr(defTheme, key);
        if (!themeValue.isEmpty())
            doSetByType(type, themeValue);
    };
    auto setGlobalFile = [&theme, &themeName, &defTheme, &themePath, this](const QString &key, const QString &type) {
        QString themeValue = theme.getStr(themeName, key);
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

    if (themePath.endsWith("custom")) {
        return setGlobalItem("AppTheme", TYPEGTK);
    }

    if (!m_desktopWallpaperCustomized) {
        setGlobalFile("Wallpaper", TYPEWALLPAPER);
    }
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
    if (type == TYPEWALLPAPER || type == TYPEGREETERBACKGROUND) {
        WallpaperContext::wallpaper_role wallpaperRole = type == TYPEWALLPAPER ? 
            WallpaperContext::wallpaper_role_desktop : WallpaperContext::wallpaper_role_lockscreen;
        auto screens = qApp->screens();
        for (const auto screen : screens) {
            setWallpaper(screen->name(), value, wallpaperRole, WallpaperContext::wallpaper_source_type_image);
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
    if (m_appearanceManager.isNull()) {
        m_appearanceManager.reset(new TreelandAppearanceManager(this));
        connect(m_appearanceManager.get(), &TreelandAppearanceManager::activeChanged, this, [this]() {
            if (m_appearanceManager->isActive()) {
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

    connect(qApp, &QGuiApplication::screenRemoved, this, [this](QScreen *screen) {
        const QString &name = screen->name();
        qCDebug(DdcPersonnalizationTreelandWorker) << "Screen removed, cleaning up wallpaper contexts for:" << name;
        delete m_wallpaperContexts.take(name);
        delete m_wallpapers.take(name);
        delete m_lockWallpapers.take(name);
    });

    connect(qApp, &QGuiApplication::screenAdded, this, [this](QScreen *screen) {
        if (!screen || !m_wallpaperManager || !m_wallpaperManager->isActive()) {
            return;
        }

        qCDebug(DdcPersonnalizationTreelandWorker) << "Screen added, initializing wallpaper context for:" << screen->name();
        getOrCreateWallpaperContext(screen->name());
    });

    PersonalizationWorker::active();

    m_globalTheme = m_model->getGlobalThemeModel()->getDefault();
}

TreelandAppearanceManager::TreelandAppearanceManager(QObject *parent)
    : QWaylandClientExtensionTemplate<TreelandAppearanceManager>(1)
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

TreelandAppearanceManager::~TreelandAppearanceManager()
{
    removeListener();
    if (isInitialized())
        QtWayland::treeland_appearance_manager_v1::destroy();
}

void TreelandAppearanceManager::addListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip addListener";
        return;
    }
    m_waylandDisplay->addRegistryListener(&handleListenerGlobal, this);
}

void TreelandAppearanceManager::removeListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip removeListener";
        return;
    }
    m_waylandDisplay->removeListener(&handleListenerGlobal, this);
}

void TreelandAppearanceManager::handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    if (interface == treeland_appearance_manager_v1_interface.name) {
        TreelandAppearanceManager *integration = static_cast<TreelandAppearanceManager *>(data);
        if (!integration) {
            qWarning() << "integration is nullptr!!!";
            return;
        }

        integration->init(registry, id, version);
    }
}

TreelandAppearance::TreelandAppearance(PersonalizationModel *model)
    : QWaylandClientExtensionTemplate<TreelandAppearance>(1)
    , m_model(model)
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
}

TreelandAppearance::~TreelandAppearance()
{
    removeListener();
    if (isInitialized())
        QtWayland::treeland_appearance_v1::destroy();
}

void TreelandAppearance::addListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip addListener";
        return;
    }
    m_waylandDisplay->addRegistryListener(&handleListenerGlobal, this);
}

void TreelandAppearance::removeListener()
{
    if (!m_waylandDisplay) {
        qWarning() << "waylandDisplay is nullptr!, skip removeListener";
        return;
    }
    m_waylandDisplay->removeListener(&handleListenerGlobal, this);
}

void TreelandAppearance::handleListenerGlobal(void *data, wl_registry *registry, uint32_t id, const QString &interface, uint32_t version)
{
    if (interface == treeland_appearance_v1_interface.name) {
        TreelandAppearance *integration = static_cast<TreelandAppearance *>(data);
        if (!integration) {
            qWarning() << "integration is nullptr!!!";
            return;
        }

        integration->init(registry, id, version);
    }
}

void TreelandAppearance::treeland_appearance_v1_cursor_theme(const QString &)
{
}

void TreelandAppearance::treeland_appearance_v1_cursor_size(uint32_t, uint32_t)
{
}

void TreelandAppearance::treeland_appearance_v1_font(const QString &)
{
}

void TreelandAppearance::treeland_appearance_v1_monospace_font(const QString &)
{
}

void TreelandAppearance::treeland_appearance_v1_font_size(uint32_t)
{
}

void TreelandAppearance::treeland_appearance_v1_icon_theme(const QString &)
{
}

void TreelandAppearance::treeland_appearance_v1_accent_color(uint32_t, uint32_t, uint32_t)
{
}

void TreelandAppearance::treeland_appearance_v1_window_opacity(wl_fixed_t)
{
}

void TreelandAppearance::treeland_appearance_v1_color_scheme(uint32_t scheme)
{
    Q_EMIT colorSchemeChanged(scheme);
}

void TreelandAppearance::treeland_appearance_v1_titlebar_height(uint32_t)
{
}

void TreelandAppearance::treeland_appearance_v1_corner_radius(uint32_t)
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

WallpaperManager::~WallpaperManager()
{
    removeListener();
    if (isInitialized())
        QtWayland::treeland_wallpaper_manager_v1::destroy();
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

WallpaperContext::WallpaperContext(struct ::treeland_wallpaper_v1 *context)
    : QWaylandClientExtensionTemplate<WallpaperContext>(1)
    , QtWayland::treeland_wallpaper_v1(context)
{

}

WallpaperContext::~WallpaperContext()
{
    if (isInitialized())
        QtWayland::treeland_wallpaper_v1::destroy();
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
    const char *errorDesc = "unknown error";
    switch (error) {
    case error_already_used:
        errorDesc = "source already configured";
        break;
    case error_invalid_source:
        errorDesc = "source is invalid, unsupported, or could not be processed";
        break;
    case error_permission_denied:
        errorDesc = "permission denied, check file permissions";
        break;
    }
    qCWarning(DdcPersonnalizationTreelandWorker) << "wallpaper failed:" << file_source << "-" << errorDesc;
    Q_EMIT wallpaperFailed(file_source, error);
}

#endif
