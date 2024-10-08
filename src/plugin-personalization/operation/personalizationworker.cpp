//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "personalizationworker.h"
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

#include <DConfig>

DCORE_USE_NAMESPACE
Q_LOGGING_CATEGORY(DdcPersonalWorker, "dcc-personal-workder")

static const std::vector<int> OPACITY_SLIDER{ 0, 25, 40, 55, 70, 85, 100 };

const QList<int> FontSizeList{ 11, 12, 13, 14, 15, 16, 18, 20 };

const QString ORG_KDE_KWIN_DECORATION = QStringLiteral("org.kde.kwin.decoration");
const QString ORG_KDE_KWIN_DECORATION_TITLEBAR = QStringLiteral("org.kde.kwin.decoration.titlebar");
const QString ORG_KDE_KWIN_COMPOSITING = QStringLiteral("org.kde.kwin.compositing");
const QString ORG_DEEPIN_CONTROL_CENTER = QStringLiteral("org.deepin.dde.control-center");
const QString CONTROL_CENTER_PERSONALIZATION = QStringLiteral("org.deepin.dde.control-center.personalization");
const QString ORG_KDE_KWIN = QStringLiteral("org.kde.kwin");

const QString SCROLLBAR_POLICY_KEY = QStringLiteral("scrollbarPolicyStatus");
const QString COMPACT_MODE_DISPLAY_KEY = QStringLiteral("compactDisplayStatus");
const QString WINDOW_EFFECT_TYPE_KEY = QStringLiteral("user_type");
const QString TITLE_BAR_DEFAULT_HEIGHT_KEY = QStringLiteral("defaultTitlebarHeight");
const QString TITLE_BAR_HEIGHT_SUPPORT_COMPACT_DISPLAY = QStringLiteral("titleBarHeightSupportCompactDisplay");
const QString TITLE_BAR_HEIGHT_KEY = QStringLiteral("titlebarHeight");
const QString EffectMoveWindowArg = "kwin4_effect_translucency";

const int RENDER_DPI = 72;
const double DPI = 96;

double ptToPx(double pt)
{
    double px = pt / RENDER_DPI * DPI + 0.5;
    return px;
}

double pxToPt(double px)
{
    double pt = px * RENDER_DPI / DPI;
    return pt;
}

PersonalizationWorker::PersonalizationWorker(PersonalizationModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_personalizationDBusProxy(new PersonalizationDBusProxy(this))
    , m_kwinTitleBarConfig(DConfig::create(ORG_KDE_KWIN_DECORATION, ORG_KDE_KWIN_DECORATION_TITLEBAR, "", this))
    , m_kwinCompositingConfig(DConfig::create(ORG_KDE_KWIN, ORG_KDE_KWIN_COMPOSITING, "", this))
    , m_personalizationConfig(DConfig::create(ORG_DEEPIN_CONTROL_CENTER, CONTROL_CENTER_PERSONALIZATION, "", this))
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

    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::WMChanged, this, &PersonalizationWorker::onToggleWM);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::OpacityChanged, this, &PersonalizationWorker::refreshOpacity);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::QtActiveColorChanged, this, &PersonalizationWorker::refreshActiveColor);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::compositingAllowSwitchChanged, this, &PersonalizationWorker::onCompositingAllowSwitch);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::compositingEnabledChanged, this, &PersonalizationWorker::onWindowWM);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::WindowRadiusChanged, this, &PersonalizationWorker::onWindowRadiusChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::DTKSizeModeChanged, this, &PersonalizationWorker::onCompactDisplayChanged);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::scrollBarPolicyChanged, this, &PersonalizationWorker::setScrollBarPolicy);
    connect(m_personalizationDBusProxy, &PersonalizationDBusProxy::Changed, this, [this](const QString &propertyName, const QString &value) {
        qCDebug(DdcPersonalWorker) << "ChangeProperty is " << propertyName << "; value is" << value;
        if (propertyName == "globaltheme") {
            refreshTheme();
        }
    });

    connect(m_kwinTitleBarConfig, &DConfig::valueChanged, this, &PersonalizationWorker::onKWinTitleBarConfigChanged);
    connect(m_kwinCompositingConfig, &DConfig::valueChanged, this, &PersonalizationWorker::onKWinCompositingConfigChanged);
    connect(m_personalizationConfig, &DConfig::valueChanged, this, &PersonalizationWorker::onPersonalizationConfigChanged);

    m_personalizationDBusProxy->isEffectLoaded("magiclamp", this, SLOT(onMiniEffectChanged(bool)));

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

    refreshWMState();
    refreshOpacity(m_personalizationDBusProxy->opacity());
    refreshActiveColor(m_personalizationDBusProxy->qtActiveColor());
    onCompositingAllowSwitch(m_personalizationDBusProxy->compositingAllowSwitch());

    m_model->getWindowModel()->setDefault(m_personalizationDBusProxy->gtkTheme());
    m_model->getIconModel()->setDefault(m_personalizationDBusProxy->iconTheme());
    m_model->getMouseModel()->setDefault(m_personalizationDBusProxy->cursorTheme());
    m_model->getGlobalThemeModel()->setDefault(m_personalizationDBusProxy->globalTheme());
    m_model->getMonoFontModel()->setFontName(m_personalizationDBusProxy->monospaceFont());
    m_model->getStandFontModel()->setFontName(m_personalizationDBusProxy->standardFont());
    m_model->setWindowRadius(m_personalizationDBusProxy->windowRadius());
    m_model->getFontSizeModel()->setFontSize(ptToPx(m_personalizationDBusProxy->fontSize()));
    m_model->setCompactDisplay(m_personalizationDBusProxy->getDTKSizeMode());
    m_model->setScrollBarPolicy(m_personalizationDBusProxy->getScrollBarPolicy());

    int titleBarHight = m_kwinTitleBarConfig->value(TITLE_BAR_HEIGHT_KEY).toInt();
    m_model->setTitleBarHeight(titleBarHight);
    int titleBarDefHight = m_kwinTitleBarConfig->value(TITLE_BAR_DEFAULT_HEIGHT_KEY).toInt();
    m_model->setTitleBarDefaultHeight(titleBarDefHight);
    int windowEffectType = m_kwinCompositingConfig->value(WINDOW_EFFECT_TYPE_KEY).toInt();
    m_model->setWindowEffectType(windowEffectType);
    QString scrollbarConfig = m_personalizationConfig->value(SCROLLBAR_POLICY_KEY).toString();
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

void PersonalizationWorker::refreshWMState()
{
    m_personalizationDBusProxy->CurrentWM(this, SLOT(onToggleWM(const QString &)));
    m_model->setIsMoveWindow(m_personalizationDBusProxy->isEffectLoaded(EffectMoveWindowArg));
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

void PersonalizationWorker::onToggleWM(const QString &wm)
{
    qCDebug(DdcPersonalWorker) << "onToggleWM: " << wm;
    m_model->setIs3DWm(wm == "deepin wm");
}

void PersonalizationWorker::onWindowWM(bool value)
{
    qDebug() << "onWindowWM: " << value;
    m_model->setIs3DWm(value);
}

void PersonalizationWorker::onMiniEffectChanged(bool value)
{
    m_model->setMiniEffect(value ? 1 : 0);
}

void PersonalizationWorker::onWindowRadiusChanged(int value)
{
    m_model->setWindowRadius(value);
}

void PersonalizationWorker::onCompositingAllowSwitch(bool value)
{
    m_model->setCompositingAllowSwitch(value);
}

void PersonalizationWorker::onCompactDisplayChanged(int value)
{
    m_model->setCompactDisplay(value);
}

void PersonalizationWorker::onWindowEffectChanged(int value)
{
    m_model->setWindowEffectType(value);
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

void PersonalizationWorker::setTitleBarHeight(int value)
{
    m_kwinTitleBarConfig->setValue(TITLE_BAR_HEIGHT_KEY, value);
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

bool PersonalizationWorker::allowSwitchWM()
{
    return m_personalizationDBusProxy->AllowSwitch();
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

void PersonalizationWorker::setFontSize(const int value)
{
    m_personalizationDBusProxy->setFontSize(pxToPt(value));
}

void PersonalizationWorker::switchWM()
{
    //check is allowed to switch wm
    bool allow = allowSwitchWM();
    if (!allow)
        return;

    m_personalizationDBusProxy->RequestSwitchWM();
}

void PersonalizationWorker::windowSwitchWM(bool value)
{
    m_personalizationDBusProxy->setCompositingEnabled(value);
}

void PersonalizationWorker::setWindowEffect(int value)
{
    qCWarning(DdcPersonalWorker) << "windowSwitchWM switch to: " << value;
    m_kwinCompositingConfig->setValue(WINDOW_EFFECT_TYPE_KEY, value);
}

void PersonalizationWorker::movedWindowSwitchWM(bool value)
{
    if (value) {
        m_personalizationDBusProxy->loadEffect(EffectMoveWindowArg);
    } else {
        m_personalizationDBusProxy->unloadEffect(EffectMoveWindowArg);
    }

    //设置kwin接口后, 等待50ms给kwin反应，根据isEffectLoaded返回值确定真实状态
    QTimer::singleShot(50, [this] {
        bool isLoaded =  m_personalizationDBusProxy->isEffectLoaded(EffectMoveWindowArg);
        qCDebug(DdcPersonalWorker) << "Moved window switch WM, load effect translucency: " << isLoaded;
        m_model->setIsMoveWindow(isLoaded);
        // m_model->setIsMoveWindowDconfig(value);
    });
}

void PersonalizationWorker::setOpacity(int opacity)
{
    m_personalizationDBusProxy->setOpacity(sliderValutToOpacity(opacity));
}

void PersonalizationWorker::setMiniEffect(int effect)
{
    switch (effect) {
    case 0:
        qCDebug(DdcPersonalWorker) << "scale";
        m_personalizationDBusProxy->unloadEffect("magiclamp");
        m_model->setMiniEffect(effect);
        break;
    case 1:
        qCDebug(DdcPersonalWorker) << "magiclamp";
        m_personalizationDBusProxy->loadEffect("magiclamp");
        m_model->setMiniEffect(effect);
        break;
    default:
        break;
    }
}

void PersonalizationWorker::setActiveColor(const QString &hexColor)
{
    m_personalizationDBusProxy->setQtActiveColor(hexColor);
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

    m_personalizationDBusProxy->setDTKSizeMode(int(value));
}


void PersonalizationWorker::setScrollBarPolicy(int policy)
{
    m_personalizationDBusProxy->setScrollBarPolicy( policy);
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

void PersonalizationWorker::onKWinTitleBarConfigChanged(const QString &key)
{
    if (key == TITLE_BAR_HEIGHT_KEY) {
        int value = m_kwinTitleBarConfig->value(key).toInt();
        m_model->setTitleBarHeight(value);
    } else if (key == WINDOW_EFFECT_TYPE_KEY) {
        int value = m_kwinTitleBarConfig->value(key).toInt();
        m_model->setWindowEffectType(value);
    }
}

void PersonalizationWorker::onKWinCompositingConfigChanged(const QString &key)
{
    if (key == WINDOW_EFFECT_TYPE_KEY) {
        int windowEffectType = m_kwinCompositingConfig->value(WINDOW_EFFECT_TYPE_KEY).toInt();
        m_model->setWindowEffectType(windowEffectType);
    }
}

void PersonalizationWorker::onPersonalizationConfigChanged(const QString &key)
{
    if (key == SCROLLBAR_POLICY_KEY) {
        QString value = m_personalizationConfig->value(key).toString();
        m_model->setScrollBarPolicyConfig(value);
    } else if (key == COMPACT_MODE_DISPLAY_KEY) {
        QString value = m_personalizationConfig->value(key).toString();
        m_model->setCompactDisplayConfig(value);
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

void PersonalizationWorker::setAppearanceTheme(const QString &id)
{
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
