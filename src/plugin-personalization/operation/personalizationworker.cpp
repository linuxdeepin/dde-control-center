//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationworker.h"
#include "operation/personalizationexport.hpp"
#include "operation/screensaverprovider.h"
#include "operation/wallpaperprovider.h"
#include "personalizationdbusproxy.h"
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"
#include "operation/personalizationmodel.h"
#include "utils.hpp"

#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QJsonArray>
#include <QCollator>
#include <QJsonDocument>
#include <QDBusError>
#include <QLoggingCategory>
#include <QTimer>
#include <QDir>
#include <QTemporaryFile>

#include <DConfig>
#include <DDBusSender>

DCORE_USE_NAMESPACE
Q_LOGGING_CATEGORY(DdcPersonalWorker, "dcc-personal-worker")

#define SOLID_PREFIX "solid::"

static const std::vector<int> OPACITY_SLIDER{ 0, 25, 40, 55, 70, 85, 100 };

const QList<int> FontSizeList{ 11, 12, 13, 14, 15, 16, 18, 20 };

constexpr auto ORG_DEEPIN_CONTROL_CENTER = "org.deepin.dde.control-center";
constexpr auto CONTROL_CENTER_PERSONALIZATION = "org.deepin.dde.control-center.personalization";
constexpr auto DTK_PREFERENCE_NAME = "org.deepin.dtk.preference";
constexpr auto SCROLLBAR_POLICY_CONFIG_KEY = "scrollbarPolicyStatus";
constexpr auto COMPACT_MODE_DISPLAY_KEY = "compactDisplayStatus";
constexpr auto TITLE_BAR_HEIGHT_SUPPORT_COMPACT_DISPLAY = "titleBarHeightSupportCompactDisplay";
constexpr auto SIZE_MODE_KEY = "sizeMode";
constexpr auto SCROLLBAR_POLICY_KEY = "scrollBarPolicy";

PersonalizationWorker::PersonalizationWorker(PersonalizationModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_personalizationDBusProxy(new PersonalizationDBusProxy(this))
    , m_wallpaperWorker(new WallpaperProvider(m_personalizationDBusProxy, m_model, this))
    , m_screenSaverProvider(new ScreensaverProvider(m_personalizationDBusProxy, m_model, this))
    , m_personalizationConfig(DConfig::create(ORG_DEEPIN_CONTROL_CENTER, CONTROL_CENTER_PERSONALIZATION, "", this))
    , m_dtkConfig(DConfig::createGeneric(DTK_PREFERENCE_NAME, "", this))
{
    ThemeModel *cursorTheme = m_model->getMouseModel();
    ThemeModel *windowTheme = m_model->getWindowModel();
    ThemeModel *iconTheme = m_model->getIconModel();
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();
    FontModel *fontMono = m_model->getMonoFontModel();
    FontModel *fontStand = m_model->getStandFontModel();

    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::GtkThemeChanged, windowTheme, &ThemeModel::setDefault);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::CursorThemeChanged, cursorTheme, &ThemeModel::setDefault);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::IconThemeChanged, iconTheme, &ThemeModel::setDefault);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::GlobalThemeChanged, globalTheme, &ThemeModel::setDefault);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::MonospaceFontChanged, fontMono, &FontModel::setFontName);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::StandardFontChanged, fontStand, &FontModel::setFontName);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::FontSizeChanged, this, &PersonalizationWorker::FontSizeChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::Refreshed, this, &PersonalizationWorker::onRefreshedChanged);

    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::OpacityChanged, this, &PersonalizationWorker::refreshOpacity);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::QtActiveColorChanged, this, &PersonalizationWorker::refreshActiveColor);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::WindowRadiusChanged, this, &PersonalizationWorker::onWindowRadiusChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::WallpaperURlsChanged, this, &PersonalizationWorker::onWallpaperUrlsChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::currentScreenSaverChanged, this, &PersonalizationWorker::onCurrentScreenSaverChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::lockScreenAtAwakeChanged, this, &PersonalizationWorker::onLockScreenAtAwakeChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::linePowerScreenSaverTimeoutChanged, this, &PersonalizationWorker::onLinePowerScreenSaverTimeoutChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::WallpaperSlideShowChanged, this, &PersonalizationWorker::onWallpaperSlideShowChanged);

    connect(m_wallpaperWorker, &WallpaperProvider::fetchFinish, this, &PersonalizationWorker::updateWallpaperSelected);

    connect(qApp, &QGuiApplication::screenAdded, this, &PersonalizationWorker::onScreensChanged);
    connect(qApp, &QGuiApplication::screenRemoved, this, &PersonalizationWorker::onScreensChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::Changed, this, [this](const QString &propertyName, const QString &value) {
        qCDebug(DdcPersonalWorker) << "ChangeProperty is " << propertyName << "; value is" << value;
        if (propertyName == "globaltheme") {
            refreshTheme();
        }
    });

    connect(m_personalizationConfig, &DConfig::valueChanged, this, &PersonalizationWorker::onPersonalizationConfigChanged);
    connect(m_dtkConfig, &DConfig::valueChanged, this, &PersonalizationWorker::onDTKConfigChanged);

    m_themeModels["gtk"] = windowTheme;
    m_themeModels["icon"] = iconTheme;
    m_themeModels["cursor"] = cursorTheme;
    m_themeModels["globaltheme"] = globalTheme;
    m_fontModels["standardfont"] = fontStand;
    m_fontModels["monospacefont"] = fontMono;
}

void PersonalizationWorker::active()
{
    m_personalizationDBusProxy->blockSignals(false);

    m_wallpaperWorker->fetchData();
    m_screenSaverProvider->fecthData();

    refreshOpacity(m_personalizationDBusProxy->opacity());
    refreshActiveColor(m_personalizationDBusProxy->qtActiveColor());
    onWallpaperUrlsChanged();
    onScreensChanged();
    onWallpaperSlideShowChanged();

    m_model->setCurrentSelectScreen(qApp->primaryScreen()->name());
    m_model->getWindowModel()->setDefault(m_personalizationDBusProxy->gtkTheme());
    m_model->getIconModel()->setDefault(m_personalizationDBusProxy->iconTheme());
    m_model->getMouseModel()->setDefault(m_personalizationDBusProxy->cursorTheme());
    m_model->getGlobalThemeModel()->setDefault(m_personalizationDBusProxy->globalTheme());
    m_model->getMonoFontModel()->setFontName(m_personalizationDBusProxy->monospaceFont());
    m_model->getStandFontModel()->setFontName(m_personalizationDBusProxy->standardFont());
    m_model->setWindowRadius(m_personalizationDBusProxy->windowRadius());
    m_model->getFontSizeModel()->setFontSize(ptToPx(m_personalizationDBusProxy->fontSize()));
    m_model->setCompactDisplay(m_personalizationDBusProxy->getDTKSizeMode());

    m_model->setScrollBarPolicy(m_dtkConfig->value(SCROLLBAR_POLICY_KEY).toInt());
    m_model->setCompactDisplay(m_dtkConfig->value(SIZE_MODE_KEY).toInt());

    m_model->setCurrentScreenSaver(m_personalizationDBusProxy->getCurrentScreenSaver());
    if (m_model->getCurrentScreenSaver() == DEEPIN_CUSTOM_SCREENSAVER) {
        m_model->setCurrentScreenSaverPicMode("default");
    } else {
        m_model->setCurrentScreenSaverPicMode("");
    }

    m_model->setLockScreenAtAwake(m_personalizationDBusProxy->getLockScreenAtAwake());
    m_model->setScreenSaverIdleTime(m_personalizationDBusProxy->getLinePowerScreenSaverTimeout());

    QString scrollbarConfig = m_personalizationConfig->value(SCROLLBAR_POLICY_CONFIG_KEY).toString();
    m_model->setScrollBarPolicyConfig(scrollbarConfig);
    QString compactDisplayConfig = m_personalizationConfig->value(COMPACT_MODE_DISPLAY_KEY).toString();
    m_model->setCompactDisplayConfig(compactDisplayConfig);
}

void PersonalizationWorker::deactive()
{
    m_personalizationDBusProxy->blockSignals(true);
}

QList<QJsonObject> PersonalizationWorker::converToList(const QString &type, const QJsonArray &array)
{
    QList<QJsonObject> list;
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        object.insert("type", QJsonValue(type));
        list.append(object);
    }
    return list;
}

void PersonalizationWorker::addList(ThemeModel *model, const QString &type, const QJsonArray &array)
{
    QList<QString> list;
    QList<QJsonObject> objList;
    QScopedPointer<DConfig> personalizationConfig(DConfig::create("org.deepin.dde.control-center", QStringLiteral("org.deepin.dde.control-center.personalization"), QString(), this));
    auto hideIconThemeList = personalizationConfig->value("hideIconThemes").toStringList();
    for (int i = 0; i != array.size(); i++) {
        QJsonObject object = array.at(i).toObject();
        if (type == "icon" && hideIconThemeList.contains(object["Name"].toString())) {
            continue;
        }
        object.insert("type", QJsonValue(type));
        if (object["Name"].toString() == "Custom") {
            object["Name"] = tr("Custom");
        }
        objList << object;
        list.append(object["Id"].toString());

        PersonalizationWatcher *watcher = new PersonalizationWatcher(this);
        watcher->setProperty("category", type);
        watcher->setProperty("id", object["Id"].toString());
        m_personalizationDBusProxy->Thumbnail(type, object["Id"].toString(), watcher, SLOT(onThumbnail(const QString &)), SLOT(errorSlot(const QDBusError &)));
    }

    for (const QJsonObject &obj : objList) {
        model->addItem(obj["Id"].toString(), obj);
    }

    for (const QString &id : model->getList().keys()) {
        if (!list.contains(id)) {
            model->removeItem(id);
        }
    }
}

void PersonalizationWorker::FontSizeChanged(const double value) const
{
    FontSizeModel *fontSizeModel = m_model->getFontSizeModel();
    int px = static_cast<int>(ptToPx(value));
    fontSizeModel->setFontSize(px);
}

void PersonalizationWorker::onGetFontFinished(const QString &category, const QString &json)
{
    setFontList(m_fontModels[category], category, json);
}

void PersonalizationWorker::onGetThemeFinished(const QString &category, const QString &json)
{
    const QJsonArray &array = QJsonDocument::fromJson(json.toUtf8()).array();
    addList(m_themeModels[category], category, array);
    if (category == "cursor") {
        m_themeModels[category]->setDefault(m_personalizationDBusProxy->cursorTheme());
    } else if (category == "icon") {
        m_themeModels[category]->setDefault(m_personalizationDBusProxy->iconTheme());
    }
}

void PersonalizationWorker::onGetPicFinished(const QString &category, const QString &id, const QString &json)
{
    m_themeModels[category]->addPic(id, json);
}

void PersonalizationWorker::onRefreshedChanged(const QString &type)
{
    if (m_themeModels.keys().contains(type)) {
        refreshThemeByType(type);
    }

    if (m_fontModels.keys().contains(type)) {
        refreshFontByType(type);
    }
}

void PersonalizationWorker::onWindowRadiusChanged(int value)
{
    m_model->setWindowRadius(value);
}

void PersonalizationWorker::onCompactDisplayChanged(int value)
{
    m_model->setCompactDisplay(value);
}

void PersonalizationWorker::onWindowEffectChanged(int value)
{
    m_model->setWindowEffectType(value);
}

void PersonalizationWorker::onScreensChanged()
{
    bool isMirror = true;
    QRect geometry = qApp->primaryScreen()->availableGeometry();
    for (const auto screen : qApp->screens()) {
        qWarning() << screen->availableGeometry() << screen->availableGeometry();
        if (geometry != screen->availableGeometry()) {
            isMirror = false;
            break;
        }
    }

    for (const auto screen : qApp->screens()) {
        connect(screen, &QScreen::geometryChanged, this, &PersonalizationWorker::onScreensChanged, Qt::UniqueConnection);
    }

    if (isMirror) {
        m_model->setScreens({qApp->primaryScreen()->name()});
        m_model->setCurrentSelectScreen(qApp->primaryScreen()->name());
        return;
    } else {
        QStringList screenNameList{};
        for (const auto &screen : qApp->screens()) {
            screenNameList << screen->name();
        }
        m_model->setScreens(screenNameList);
    };
}

void PersonalizationWorker::onCurrentScreenSaverChanged(const QString &value)
{
    m_model->setCurrentScreenSaver(value);
}

void PersonalizationWorker::onLockScreenAtAwakeChanged(bool value)
{
    m_model->setLockScreenAtAwake(value);
}

void PersonalizationWorker::onLinePowerScreenSaverTimeoutChanged(int value)
{
    m_model->setScreenSaverIdleTime(value);
}

void PersonalizationWorker::onWallpaperSlideShowChanged()
{
    QVariantMap wallpaperSlideShowMap;
    QStringList screenNameList;
    for (const auto screen : qApp->screens()) {
        screenNameList << screen->name();
    }
    for (const auto &screenName : screenNameList) {
        QString slideShow = m_personalizationDBusProxy->wallpaperSlideShow(screenName);
        wallpaperSlideShowMap.insert(screenName, slideShow);
    }
    if (!wallpaperSlideShowMap.isEmpty()) {
        m_model->setWallpaperSlideShowMap(wallpaperSlideShowMap);
    }
}

void PersonalizationWorker::updateWallpaperSelected()
{
    QStringList wallpaperList;
    auto wallpaperMap = m_model->getWallpaperMap();
    for (auto it = wallpaperMap.cbegin(); it != wallpaperMap.cend(); ++it) {
        wallpaperList << it.value().toString();
    }

    m_model->getSysWallpaperModel()->updateSelected(wallpaperList);
    m_model->getSolidWallpaperModel()->updateSelected(wallpaperList);
    m_model->getCustomWallpaperModel()->updateSelected(wallpaperList);
}

void PersonalizationWorker::onWallpaperUrlsChanged()
{
    // wallpaperUrls 存储着每个工作区和每个屏幕的壁纸, 若其改变, 需要刷新当前屏幕壁纸
    QVariantMap wallpaperMap;
    QStringList screenNameList;
    for (const auto screen : qApp->screens()) {
        screenNameList << screen->name();
    }
    for (const auto &screenName : screenNameList) {
        QString url = m_personalizationDBusProxy->getCurrentWorkSpaceBackgroundForMonitor(screenName);
        if (!url.isEmpty()) {
            wallpaperMap.insert(screenName, url);
        }
    }
    if (!wallpaperMap.isEmpty()) {
        m_model->setWallpaperMap(wallpaperMap);
    }

    updateWallpaperSelected();
}

void PersonalizationWorker::setFontList(FontModel *model, const QString &type, const QString &list)
{
    QJsonArray array = QJsonDocument::fromJson(list.toLocal8Bit().data()).array();

    QStringList l;

    for (int i = 0; i != array.size(); i++)
        l << array.at(i).toString();

    PersonalizationWatcher *watcher = new PersonalizationWatcher(this);
    watcher->setProperty("type", type);
    watcher->setProperty("FontModel", QVariant::fromValue(static_cast<void *>(model)));
    m_personalizationDBusProxy->Show(type, l, watcher, SLOT(onShow(const QString &)));
}

void PersonalizationWorker::setTitleBarHeight(int)
{

}

void PersonalizationWorker::setDiabledCompactToTitleHeight()
{
    if (m_personalizationConfig->value(TITLE_BAR_HEIGHT_SUPPORT_COMPACT_DISPLAY).toBool()) {
        m_personalizationConfig->setValue(TITLE_BAR_HEIGHT_SUPPORT_COMPACT_DISPLAY, false);
    }
}

void PersonalizationWorker::refreshTheme()
{
    for (QMap<QString, ThemeModel *>::ConstIterator it = m_themeModels.begin(); it != m_themeModels.end(); it++) {
        refreshThemeByType(it.key());
    }
}

void PersonalizationWorker::refreshThemeByType(const QString &type)
{
    PersonalizationWatcher *watcher = new PersonalizationWatcher(this);
    watcher->setProperty("category", type);
    m_personalizationDBusProxy->List(type, watcher, SLOT(onList(const QString &)), SLOT(errorSlot(const QDBusError &)));
}

void PersonalizationWorker::refreshFont()
{
    for (QMap<QString, FontModel *>::const_iterator it = m_fontModels.begin(); it != m_fontModels.end(); it++) {
        refreshFontByType(it.key());
    }

    FontSizeChanged(m_personalizationDBusProxy->fontSize());
}

bool PersonalizationWorker::checkWallpaperLockStatus()
{
    if (QFileInfo::exists("/var/lib/deepin/permission-manager/wallpaper_locked")) {
        QDBusInterface notify("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications");
        notify.asyncCall(QString("Notify"),
                         QString("dde-control-center"),   // title
                         static_cast<uint>(0),
                         QString("preferences-system"),   // icon
                         QObject::tr("This system wallpaper is locked. Please contact your admin."),
                         QString(), QStringList(), QVariantMap(), 5000);
        qCInfo(DdcPersonalWorker) << "wallpaper is locked..";
        return true;
    }
    return false;
}

void PersonalizationWorker::refreshFontByType(const QString &type)
{
    PersonalizationWatcher *watcher = new PersonalizationWatcher(this);
    watcher->setProperty("category", type);
    m_personalizationDBusProxy->List(type, watcher, SLOT(onGetFont(const QString &)), SLOT(errorSlot(const QDBusError &)));
}

void PersonalizationWorker::refreshActiveColor(const QString &color)
{
    m_model->setActiveColor(color);
}

void PersonalizationWorker::refreshOpacity(double opacity)
{
    qCDebug(DdcPersonalWorker) << QString("opacity: %1").arg(opacity);
    m_model->setOpacity(opacity);
}


double PersonalizationWorker::sliderValutToOpacity(const int value) const
{
    return static_cast<double>(value) / static_cast<double>(100);
}

void PersonalizationWorker::setDefaultByType(const QString &type, const QString &value)
{
    m_personalizationDBusProxy->Set(type, value);
}

void PersonalizationWorker::setDefault(const QJsonObject &value)
{
    //使用type去调用
    m_personalizationDBusProxy->Set(value["type"].toString(), value["Id"].toString());
}

void PersonalizationWorker::setFontSize(const int pixelSize)
{
    m_personalizationDBusProxy->setFontSize(pxToPt(pixelSize));
}

void PersonalizationWorker::setWindowEffect(int)
{

}

void PersonalizationWorker::setMovedWindowOpacity(bool)
{

}

void PersonalizationWorker::setOpacity(int opacity)
{
    m_personalizationDBusProxy->setOpacity(sliderValutToOpacity(opacity));
}

void PersonalizationWorker::setMiniEffect(int)
{

}

void PersonalizationWorker::setActiveColor(const QString &hexColor)
{
    m_personalizationDBusProxy->setQtActiveColor(hexColor);
}

void PersonalizationWorker::setActiveColors(const QString &activeColors)
{
    m_personalizationDBusProxy->setActiveColors(activeColors);
}

void PersonalizationWorker::addCustomWallpaper(const QString &url)
{
    QString lastHashPath;
    if (isURI(url)) {
        lastHashPath = m_personalizationDBusProxy->saveCustomWallpaper(currentUserName(), QUrl(url).toLocalFile());
    } else {
        lastHashPath = m_personalizationDBusProxy->saveCustomWallpaper(currentUserName(), url);
    }
    setWallpaperForMonitor(m_model->getCurrentSelectScreen(), lastHashPath, false, PersonalizationExport::Option_All);
}

void PersonalizationWorker::addSolidWallpaper(const QColor &color)
{
    QString path = QDir::tempPath() + QString("/XXXXXX-solid-color-%0%1%2.jpg").arg(QString::number(color.red(), 16))
                                                          .arg(QString::number(color.green(), 16))
                                                          .arg(QString::number(color.blue(), 16));
    // create img
    QImage img(1920, 1080, QImage::Format_ARGB32);
    img.fill(color);
    QTemporaryFile file(path);
    file.setAutoRemove(false); // 将临时文件设置为不自动删除
    if (!file.open()) {
        qCWarning(DdcPersonalWorker) << "fail to save image" << file.fileName();
        return;
    }

    img.save(&file, "JPG");

    //set to dde, and prefix solid:: to tell dde this is a solid color wallpaper.
    const QString &hashPath = m_personalizationDBusProxy->saveCustomWallpaper(currentUserName(), SOLID_PREFIX + file.fileName());
    setWallpaperForMonitor(m_model->getCurrentSelectScreen(), hashPath, false, PersonalizationExport::Option_All);
}

void PersonalizationWorker::deleteWallpaper(const QString &str)
{
    qCWarning(DdcPersonalWorker) << "delete wallpaper" << str;
    if (isURI(str)) {
        m_personalizationDBusProxy->deleteCustomWallpaper(currentUserName(), QUrl(str).toLocalFile());
    } else {
        m_personalizationDBusProxy->deleteCustomWallpaper(currentUserName(), str);
    }
}

void PersonalizationWorker::setScreenSaver(const QString &value)
{
    m_personalizationDBusProxy->setCurrentScreenSaver(value);
    onCurrentScreenSaverChanged(value);
}

void PersonalizationWorker::setWallpaperSlideShow(const QString &monitorName, const QString &sliderShow)
{
    m_personalizationDBusProxy->setWallpaperSlideShow(monitorName, sliderShow);
}

void PersonalizationWorker::setCurrentScreenSaverPicMode(const QString &mode)
{
    m_model->setCurrentScreenSaverPicMode(mode);
}

void PersonalizationWorker::requestScreenSaverConfig(const QString &name)
{
    m_personalizationDBusProxy->requestScreenSaverConfig(name);
}

void PersonalizationWorker::startScreenSaverPreview()
{
    // Directly use start for preview, which can automatically release the screensaver state after moving the mouse
    m_personalizationDBusProxy->startScreenSaver();
}

void PersonalizationWorker::stopScreenSaverPreview()
{
    m_personalizationDBusProxy->stopScreenSaver();
}

void PersonalizationWorker::setLockScreenAtAwake(bool value)
{
    m_model->setLockScreenAtAwake(value);
    m_personalizationDBusProxy->setLockScreenAtAwake(value);
}

void PersonalizationWorker::setScreenSaverIdleTime(int value)
{
    m_model->setScreenSaverIdleTime(value);
    m_personalizationDBusProxy->setLinePowerScreenSaverTimeout(value);
    m_personalizationDBusProxy->setBatteryScreenSaverTimeout(value);
}

void PersonalizationWorker::setWindowRadius(int radius)
{
    m_personalizationDBusProxy->setWindowRadius(radius);
}

void PersonalizationWorker::setCompactDisplay(bool value)
{
    // 如果标题栏高度支持紧凑模式，则开/关紧凑模式需要与标题栏高度联动, 应与前端数值保持一致
    static const QVector<uint8_t> TitleHeightList = {24, 32, 40, 50};
    bool isTitleBarHeightSupported = m_personalizationConfig->value(TITLE_BAR_HEIGHT_SUPPORT_COMPACT_DISPLAY).toBool();

    if (isTitleBarHeightSupported) {
        int index = TitleHeightList.indexOf(m_model->getTitleBarHeight());
        int tarHeight = value ? TitleHeightList.value(index - 1) : TitleHeightList.value(index + 1);
        if (TitleHeightList.contains(tarHeight)) {
            setTitleBarHeight(tarHeight);
        } 
    }

    m_dtkConfig->setValue(SIZE_MODE_KEY, value);
    // TODO delete
    m_personalizationDBusProxy->setDTKSizeMode(int(value));
}


void PersonalizationWorker::setScrollBarPolicy(int policy)
{
    m_dtkConfig->setValue(SCROLLBAR_POLICY_KEY, policy);
    // TODO delete
    m_personalizationDBusProxy->setScrollBarPolicy( policy);
}

void PersonalizationWorker::goDownloadTheme()
{
    DDBusSender().interface("com.home.appstore.client")
    .path("/com/home/appstore/client")
    .service("com.home.appstore.client")
    .method("openBusinessUri")
    .arg(QString("searchApp?keyword=theme")).call();
}

template<typename T>
T PersonalizationWorker::toSliderValue(std::vector<T> list, T value)
{
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
        if (value < *it) {
            return (--it) - list.begin();
        }
    }

    return list.end() - list.begin();
}

void PersonalizationWorker::onPersonalizationConfigChanged(const QString &key)
{
    if (key == SCROLLBAR_POLICY_CONFIG_KEY) {
        QString value = m_personalizationConfig->value(key).toString();
        m_model->setScrollBarPolicyConfig(value);
    } else if (key == COMPACT_MODE_DISPLAY_KEY) {
        QString value = m_personalizationConfig->value(key).toString();
        m_model->setCompactDisplayConfig(value);
    }
}

void PersonalizationWorker::onDTKConfigChanged(const QString &key)
{
    qCWarning(DdcPersonalWorker) << "PersonalizationWorker::onDTKConfigChanged" << key << m_dtkConfig->value(key);
    if (key == SIZE_MODE_KEY) {
        m_model->setCompactDisplay(m_dtkConfig->value(key).toBool());
    } else if (key == SCROLLBAR_POLICY_KEY) {
        m_model->setScrollBarPolicy(m_dtkConfig->value(key).toInt());
    }
}

void PersonalizationWorker::setGlobalTheme(const QString &themeId)
{
    qDebug() << "applied global theme" << themeId;
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();
    QString mode;
    (void)getGlobalThemeId(globalTheme->getDefault(), mode);

    const QMap<QString, QJsonObject> &itemList = globalTheme->getList();
    if (itemList.contains(themeId))
        setDefaultByType(itemList.value(themeId)["type"].toString(), themeId + mode);
}

void PersonalizationWorker::setAppearanceTheme(const QString &id, bool keepAuto)
{
    Q_UNUSED(keepAuto)
    ThemeModel *globalTheme = m_model->getGlobalThemeModel();
    QString mode;
    QString themeId = getGlobalThemeId(globalTheme->getDefault(), mode);
    const QMap<QString, QJsonObject> &itemList = globalTheme->getList();
    if (itemList.contains(themeId)) {
        setDefaultByType(itemList.value(themeId)["type"].toString(),
                                    themeId + id);
    }
}

void PersonalizationWorker::setIconTheme(const QString &id)
{
    for (auto &object : m_model->getIconModel()->getList()) {
        if (object.value("Id").toString() == id) {
            setDefault(object);
            return;
        }
    }
}
void PersonalizationWorker::setCursorTheme(const QString &id)
{
    for (auto &object : m_model->getMouseModel()->getList()) {
        if (object.value("Id").toString() == id) {
            setDefault(object);
            return;
        }
    }
}

void PersonalizationWorker::setWallpaperForMonitor(const QString &screen, const QString &url, bool isDark, PersonalizationExport::WallpaperSetOption option)
{
    if (option == PersonalizationExport::Option_Desktop) {
        setBackgroundForMonitor(screen, url, isDark);
    } else if (option == PersonalizationExport::Option_Lock) {
        setLockBackForMonitor(screen, url, isDark);
    } else if (option == PersonalizationExport::Option_All) {
        setBackgroundForMonitor(screen, url, isDark);
        setLockBackForMonitor(screen, url, isDark);
    }
}

void PersonalizationWorker::setBackgroundForMonitor(const QString &, const QString &, bool )
{

}

void PersonalizationWorker::setLockBackForMonitor(const QString &, const QString &, bool)
{

}

PersonalizationWatcher::PersonalizationWatcher(PersonalizationWorker *work)
    : QObject(work)
    , m_work(work)
{
}

void PersonalizationWatcher::onShow(const QString &json)
{
    deleteLater();
    QJsonArray arrayValue = QJsonDocument::fromJson(json.toLocal8Bit().data()).array();

    QList<QJsonObject> list = m_work->converToList(property("type").toString(), arrayValue);
    // sort for display name
    std::sort(list.begin(), list.end(), [=](const QJsonObject &obj1, const QJsonObject &obj2) {
        QCollator qc;
        return qc.compare(obj1["Name"].toString(), obj2["Name"].toString()) < 0;
    });
    FontModel *model = static_cast<FontModel *>(property("FontModel").value<void *>());
    model->setFontList(list);
}

void PersonalizationWatcher::onList(const QString &json)
{
    m_work->onGetThemeFinished(property("category").toString(), json);
    deleteLater();
}

void PersonalizationWatcher::onGetFont(const QString &json)
{
    m_work->onGetFontFinished(property("category").toString(), json);
    deleteLater();
}

void PersonalizationWatcher::onThumbnail(const QString &json)
{
    m_work->onGetPicFinished(property("category").toString(), property("id").toString(), json);
    deleteLater();
}

void PersonalizationWatcher::errorSlot(const QDBusError &err)
{
    qCInfo(DdcPersonalWorker) << err;
    deleteLater();
}
