//SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "x11worker.h"
#include "operation/personalizationworker.h"

#include <QLoggingCategory>
#include <QTimer>
#include <DConfig>

DCORE_USE_NAMESPACE
Q_LOGGING_CATEGORY(DdcPersonnalizationX11Worker, "dcc-personalization-X11-woker")

constexpr auto ORG_KDE_KWIN_DECORATION = "org.kde.kwin.decoration";
constexpr auto ORG_KDE_KWIN_DECORATION_TITLEBAR = "org.kde.kwin.decoration.titlebar";
constexpr auto TITLE_BAR_HEIGHT_KEY = "titlebarHeight";
constexpr auto DEFAULT_TITLE_BAR_HEIGHT_KEY = "defaultTitlebarHeight";
constexpr auto WINDOW_EFFECT_TYPE_KEY = "user_type";
constexpr auto ORG_KDE_KWIN = "org.kde.kwin";
constexpr auto ORG_KDE_KWIN_COMPOSITING = "org.kde.kwin.compositing";
constexpr auto EffectMoveWindowArg = "kwin4_effect_translucency";
constexpr auto EffectMiniLampArg = "magiclamp";
constexpr auto EffectMiniScaleArg = "kwin4_effect_scale";

X11Worker::X11Worker(PersonalizationModel *model, QObject *parent)
    : PersonalizationWorker(model, parent)
    , m_kwinTitleBarConfig(DConfig::create(ORG_KDE_KWIN_DECORATION, ORG_KDE_KWIN_DECORATION_TITLEBAR, "", this))
    , m_kwinCompositingConfig(DConfig::create(ORG_KDE_KWIN, ORG_KDE_KWIN_COMPOSITING, "", this))

{
    connect(m_kwinTitleBarConfig, &DConfig::valueChanged, this, &X11Worker::onKWinConfigChanged);
    connect(m_kwinCompositingConfig, &DConfig::valueChanged, this, &X11Worker::onKWinConfigChanged);
}

void X11Worker::active()
{
    PersonalizationWorker::active();
    onTitleHeightChanged();
    int windowEffectType = m_kwinCompositingConfig->value(WINDOW_EFFECT_TYPE_KEY).toInt();
    m_model->setWindowEffectType(windowEffectType);
    m_personalizationDBusProxy->isEffectLoaded(EffectMiniLampArg, this, SLOT(onMiniEffectChanged(bool)));
    m_model->setIsMoveWindow(m_personalizationDBusProxy->isEffectLoaded(EffectMoveWindowArg));

    QStringList supportEffects;
    if (m_personalizationDBusProxy->isEffectSupported(EffectMiniLampArg)) {
        supportEffects.append(EffectMiniLampArg);
    }
    if (m_personalizationDBusProxy->isEffectSupported(EffectMiniScaleArg)) {
        supportEffects.append(EffectMiniScaleArg);
    }
    if (m_personalizationDBusProxy->isEffectSupported(EffectMoveWindowArg)) {
        supportEffects.append(EffectMoveWindowArg);
    }
    m_model->setSupportEffects(supportEffects);
}

void X11Worker::setTitleBarHeight(int value)
{
    if (m_kwinTitleBarConfig->value(TITLE_BAR_HEIGHT_KEY).toInt() != value) {
        m_kwinTitleBarConfig->setValue(TITLE_BAR_HEIGHT_KEY, value);
    }
}

void X11Worker::setWindowEffect(int value)
{
    qCDebug(DdcPersonnalizationX11Worker) << "windowSwitchWM switch to: " << value;
    m_kwinCompositingConfig->setValue(WINDOW_EFFECT_TYPE_KEY, value);
}

void X11Worker::onKWinConfigChanged(const QString &key)
{
    if (key == TITLE_BAR_HEIGHT_KEY) {
        onTitleHeightChanged();
    } else if (key == WINDOW_EFFECT_TYPE_KEY) {
        int value = m_kwinCompositingConfig->value(key).toInt();
        m_model->setWindowEffectType(value);
    }
}

void X11Worker::onTitleHeightChanged()
{
    int titleBarHight = m_kwinTitleBarConfig->value(TITLE_BAR_HEIGHT_KEY).toInt();
    if (titleBarHight < 24 || titleBarHight > 50) {
        titleBarHight = m_kwinTitleBarConfig->value(DEFAULT_TITLE_BAR_HEIGHT_KEY).toInt();
    }
    m_model->setTitleBarHeight(titleBarHight);
}

void X11Worker::onMiniEffectChanged(bool value)
{
    m_model->setMiniEffect(value ? 1 : 0);
}

void X11Worker::setMovedWindowOpacity(bool value)
{
    if (value) {
        m_personalizationDBusProxy->loadEffect(EffectMoveWindowArg);
    } else {
        m_personalizationDBusProxy->unloadEffect(EffectMoveWindowArg);
    }

    //设置kwin接口后, 等待50ms给kwin反应，根据isEffectLoaded返回值确定真实状态
    QTimer::singleShot(50, [this] {
        bool isLoaded =  m_personalizationDBusProxy->isEffectLoaded(EffectMoveWindowArg);
        qCDebug(DdcPersonnalizationX11Worker) << "Moved window switch WM, load effect translucency: " << isLoaded;
        m_model->setIsMoveWindow(isLoaded);
    });
}

void X11Worker::setMiniEffect(int effect)
{
    switch (effect) {
    case 0:
        qCDebug(DdcPersonnalizationX11Worker) << EffectMiniScaleArg;
        m_personalizationDBusProxy->unloadEffect(EffectMiniLampArg);
        m_model->setMiniEffect(effect);
        break;
    case 1:
        qCDebug(DdcPersonnalizationX11Worker) << EffectMiniLampArg;
        m_personalizationDBusProxy->loadEffect(EffectMiniLampArg);
        m_model->setMiniEffect(effect);
        break;
    default:
        break;
    }
}

void X11Worker::setWallpaperForMonitor(const QString &screen, const QString &url, bool isDark, PersonalizationExport::WallpaperSetOption option)
{
    if (checkWallpaperLockStatus()) {
        return;
    }

    if (option == PersonalizationExport::Option_Desktop) {
        setBackgroundForMonitor(screen, url, isDark);
    } else if (option == PersonalizationExport::Option_Lock) {
        setLockBackForMonitor(screen, url, isDark);
    } else if (option == PersonalizationExport::Option_All) {
        setBackgroundForMonitor(screen, url, isDark);
        setLockBackForMonitor(screen, url, isDark);
    }
}

void X11Worker::setBackgroundForMonitor(const QString &screenName, const QString &url, bool isDark)
{
    Q_UNUSED(isDark)
    qCDebug(DdcPersonnalizationX11Worker) << "setMonitorBackground " << screenName << url;
    if (screenName.isEmpty() || url.isEmpty())
        return;

    m_personalizationDBusProxy->SetCurrentWorkspaceBackgroundForMonitor(url, screenName);
}

void X11Worker::setLockBackForMonitor(const QString &screenName, const QString &url, bool isDark)
{
    Q_UNUSED(isDark)
    qCDebug(DdcPersonnalizationX11Worker) << "setGreeterBackground " << screenName << url;
    if (screenName.isEmpty() || url.isEmpty())
        return;

    m_personalizationDBusProxy->SetGreeterBackground(url);
}
