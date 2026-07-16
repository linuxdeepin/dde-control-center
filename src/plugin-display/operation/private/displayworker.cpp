// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "displayworker.h"

#include "displaymodel.h"

#include <Output.h>
#include <OutputManager.h>
#include <Registry.h>
#include <TreeLandOutputManager.h>
#include <WallpaperManager.h>
#include <WayQtUtils.h>
#include <dconfig.h>

#include <QCryptographicHash>
#include <QDateTime>
#include <QDBusPendingCallWatcher>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QPointer>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QtConcurrent/QtConcurrent>
#include <libffmpegthumbnailer/videothumbnailer.h>

Q_LOGGING_CATEGORY(DdcDisplayWorker, "dcc-display-worker")

const QString DisplayInterface("org.deepin.dde.Display1");
static const QString EyeProtectionConfig = "/usr/share/dde-wloutput-daemon/eyeprotection.xml";
static const QString SysLiveWallpaperDir = "/usr/share/wallpapers/deepin-livewallpapers";

static const QStringList videoSuffixes = { "mp4", "mov", "avi", "webm", "mkv" };

static bool isVideoFile(const QString &path)
{
    return videoSuffixes.contains(QFileInfo(path).suffix().toLower());
}

static constexpr uint32_t WallpaperSourceTypeVideo = 1;

Q_DECLARE_METATYPE(QList<QDBusObjectPath>)
using namespace dccV25;

DisplayWorker::DisplayWorker(DisplayModel *model, QObject *parent, bool isSync)
    : QObject(parent)
    , m_model(model)
    , m_displayInter(new DisplayDBusProxy(this))
    , m_reg(nullptr)
    , m_updateScale(false)
    , m_timer(new QTimer(this))
    , m_dconfig(DTK_CORE_NAMESPACE::DConfig::create("org.deepin.dde.control-center", QStringLiteral("org.deepin.dde.control-center.display"), QString(), this))
    , m_displayDConf(DTK_CORE_NAMESPACE::DConfig::create("org.deepin.dde.daemon", QStringLiteral("org.deepin.Display"), QString(), this))
    , m_tcMaxValue(6500)
    , m_tcMinValue(2000)
    , m_defaultMode(3500)
{
    // NOTE: what will it be used?
    Q_UNUSED(isSync)
    m_timer->setSingleShot(true);
    m_timer->setInterval(200);
    initCTMData();
    if (m_displayDConf && m_displayDConf->keyList().contains("defaultTemperatureManual")){
        m_defaultMode = m_displayDConf->value("defaultTemperatureManual").toInt();
    }

    if (WQt::Utils::isTreeland()) {
        QMetaObject::invokeMethod(this, "initTreeland", Qt::QueuedConnection);
    } else {
        connect(m_displayInter, &DisplayDBusProxy::WallpaperURlsChanged, this, &DisplayWorker::updateWallpaper);
        connect(m_displayInter, &DisplayDBusProxy::WorkspaceSwitched, this, &DisplayWorker::updateWallpaper);
        connect(m_displayInter, &DisplayDBusProxy::MonitorsChanged, this, &DisplayWorker::onMonitorListChanged);
        connect(m_displayInter, &DisplayDBusProxy::BrightnessChanged, this, &DisplayWorker::onMonitorsBrightnessChanged);
        connect(m_displayInter, &DisplayDBusProxy::BrightnessChanged, model, &DisplayModel::setBrightnessMap);
        connect(m_displayInter, &DisplayDBusProxy::TouchscreensV2Changed, model, &DisplayModel::setTouchscreenList);
        connect(m_displayInter, &DisplayDBusProxy::TouchMapChanged, model, &DisplayModel::setTouchMap);
        connect(m_displayInter, &DisplayDBusProxy::ScreenHeightChanged, model, &DisplayModel::setScreenHeight);
        connect(m_displayInter, &DisplayDBusProxy::ScreenWidthChanged, model, &DisplayModel::setScreenWidth);
        connect(m_displayInter, &DisplayDBusProxy::DisplayModeChanged, model, &DisplayModel::setDisplayMode);
        connect(m_displayInter, &DisplayDBusProxy::MaxBacklightBrightnessChanged, model, &DisplayModel::setmaxBacklightBrightness);
        connect(m_displayInter, &DisplayDBusProxy::ColorTemperatureEnabledChanged, model, &DisplayModel::setColorTemperatureEnabled);
        connect(m_displayInter, &DisplayDBusProxy::ColorTemperatureModeChanged, model, &DisplayModel::setAdjustCCTmode);
        connect(m_displayInter, &DisplayDBusProxy::ColorTemperatureManualChanged, this, [this](int kelvin) {
            m_model->setColorTemperature(toColorTempPos(kelvin));
        });
        connect(m_displayInter, &DisplayDBusProxy::CustomColorTempTimePeriodChanged, model, &DisplayModel::setCustomColorTempTimePeriod);
        connect(m_displayInter, static_cast<void (DisplayDBusProxy::*)(const QString &) const>(&DisplayDBusProxy::PrimaryChanged), model, &DisplayModel::setPrimary);

        // concat screen
        connect(m_displayInter, &DisplayDBusProxy::ConcatScreenChanged, model, &DisplayModel::setIsConcatScreenMode);

        // display redSfit/autoLight
        connect(m_displayInter, &DisplayDBusProxy::HasAmbientLightSensorChanged, m_model, &DisplayModel::autoLightAdjustVaildChanged);
        connect(m_timer, &QTimer::timeout, this, [this] {
            m_displayInter->ApplyChanges().waitForFinished();
            m_displayInter->Save().waitForFinished();
        });
    }

    connect(this, &DisplayWorker::videoThumbnailReady, this, [this](const QString &videoPath, const QString &thumbnailPath) {
        auto monitors = m_videoWallpaperMonitors.value(videoPath);
        if (!thumbnailPath.isEmpty()) {
            for (auto &mon : monitors) {
                if (mon) {
                    mon->setWallpaper(thumbnailPath);
                }
            }
        }
        m_videoWallpaperMonitors.remove(videoPath);
    });
}

DisplayWorker::~DisplayWorker()
{
    qDeleteAll(m_monitors.keys());
    qDeleteAll(m_monitors.values());
}

void DisplayWorker::initTreeland()
{
    // treeland协议需要在主线程中初始化
    m_reg = new WQt::Registry(WQt::Wayland::display(), this);
    connect(m_reg, &WQt::Registry::interfaceRegistered, this, &DisplayWorker::onInterfaceRegistered);
    m_reg->setup();
}

void DisplayWorker::active()
{
    if (!WQt::Utils::isTreeland()) {
        //    m_model->setAllowEnableMultiScaleRatio(
        //        valueByQSettings<bool>(DCC_CONFIG_FILES,
        //                               "Display",
        //                               "AllowEnableMultiScaleRatio",
        //                               false));

        QDBusPendingCallWatcher *scalewatcher = new QDBusPendingCallWatcher(m_displayInter->GetScaleFactor(), this);
        connect(scalewatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScaleFinished);

        QDBusPendingCallWatcher *screenscaleswatcher = new QDBusPendingCallWatcher(m_displayInter->GetScreenScaleFactors(), this);
        connect(screenscaleswatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScreenScalesFinished);

        onMonitorsBrightnessChanged(m_displayInter->brightness());
        m_model->setBrightnessMap(m_displayInter->brightness());
        onMonitorListChanged(m_displayInter->monitors());

        m_model->setDisplayMode(m_displayInter->displayMode());
        m_model->setTouchscreenList(m_displayInter->touchscreensV2());
        m_model->setTouchMap(m_displayInter->touchMap());
        m_model->setPrimary(m_displayInter->primary());
        m_model->setScreenHeight(m_displayInter->screenHeight());
        m_model->setScreenWidth(m_displayInter->screenWidth());
        m_model->setAdjustCCTmode(m_displayInter->colorTemperatureMode());
        m_model->setColorTemperatureEnabled(m_displayInter->colorTemperatureEnabled());
        m_model->setColorTemperature(toColorTempPos(m_displayInter->colorTemperatureManual()));
        m_model->setCustomColorTempTimePeriod(m_displayInter->customColorTempTimePeriod());
        m_model->setmaxBacklightBrightness(m_displayInter->maxBacklightBrightness());
        m_model->setAutoLightAdjustIsValid(m_displayInter->hasAmbientLightSensor());

        // 初始化自动亮度
        initAutoBacklight();

        bool isRedshiftValid = true;
        QDBusReply<bool> reply = m_displayInter->SupportSetColorTemperatureSync();
        if (QDBusError::NoError == reply.error().type())
            isRedshiftValid = reply.value();
        else
            qCWarning(DdcDisplayWorker) << "Call SupportSetColorTemperature method failed: " << reply.error().message();
        m_model->setRedshiftIsValid(isRedshiftValid);
        QVariant minBrightnessValue = 0.1f;
        minBrightnessValue = m_dconfig->value("minBrightnessValue", minBrightnessValue);
        m_model->setMinimumBrightnessScale(minBrightnessValue.toDouble());
        //    m_model->setResolutionRefreshEnable(m_dccSettings->get(GSETTINGS_SHOW_MUTILSCREEN).toBool());
        //    m_model->setBrightnessEnable(m_dccSettings->get(GSETTINGS_BRIGHTNESS_ENABLE).toBool());
    }
}

void DisplayWorker::saveChanges()
{
    clearBackup();
    m_displayInter->Save().waitForFinished();
    if (m_updateScale)
        setUiScale(m_currentScale);
    m_updateScale = false;
}

void DisplayWorker::switchMode(const int mode, const QString &name)
{
    if (WQt::Utils::isTreeland()) {
        auto *voMgr = m_reg->virtualOutputManager();
        auto updateDisplayModeFromCurrentState = [this]() {
            updateVirtualOutputs();
            updateTreelandDisplayMode();
        };

        // 销毁所有已有的虚拟输出
        if (voMgr) {
            for (const auto &name : voMgr->virtualOutputs().keys()) {
                voMgr->destroyVirtualOutput(name);
            }
        }

        if (mode == MERGE_MODE) {
            if (voMgr) {
                QStringList outputNames;
                for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
                    if (it.key()->name() == m_model->primary()) {
                        outputNames.prepend(it.key()->name());
                    } else {
                        outputNames.append(it.key()->name());
                    }
                }
                if (outputNames.size() >= 2) {
                    auto createMergeVirtualOutput = [this, voMgr, outputNames]() {
                        voMgr->createVirtualOutput("ddeScreenGroup", outputNames);
                        QMap<QString, QStringList> voMap;
                        voMap.insert("ddeScreenGroup", outputNames);
                        m_model->setVirtualOutput(voMap);
                        m_model->setDisplayMode(MERGE_MODE);
                    };

                    auto *opMgr = m_reg->outputManager();
                    if (!opMgr) {
                        updateDisplayModeFromCurrentState();
                        return;
                    }

                    auto *opCfg = opMgr->createConfiguration();
                    if (!opCfg) {
                        updateDisplayModeFromCurrentState();
                        return;
                    }
                    for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
                        auto *cfgHead = opCfg->enableHead(it.value());
                        if (!cfgHead) {
                            opCfg->deleteLater();
                            updateDisplayModeFromCurrentState();
                            return;
                        }
                        if (!it.key()->enable()) {
                            WQt::OutputMode *preferMode = nullptr;
                            for (auto *mode : it.value()->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
                                preferMode = mode;
                                if (mode->isPreferred())
                                    break;
                            }
                            if (preferMode)
                                cfgHead->setMode(preferMode);
                        }
                        cfgHead->setPosition({ 0, 0 });
                    }
                    connect(opCfg, &WQt::OutputConfiguration::succeeded, this, createMergeVirtualOutput);
                    connect(opCfg, &WQt::OutputConfiguration::succeeded, opCfg, &QObject::deleteLater);
                    connect(opCfg, &WQt::OutputConfiguration::failed, this, updateDisplayModeFromCurrentState);
                    connect(opCfg, &WQt::OutputConfiguration::failed, opCfg, &QObject::deleteLater);
                    connect(opCfg, &WQt::OutputConfiguration::canceled, this, updateDisplayModeFromCurrentState);
                    connect(opCfg, &WQt::OutputConfiguration::canceled, opCfg, &QObject::deleteLater);
                    opCfg->apply();
                }
            }
        } else {
            auto *opMgr = m_reg->outputManager();
            if (!opMgr) {
                updateDisplayModeFromCurrentState();
                return;
            }

            auto *opCfg = opMgr->createConfiguration();
            if (!opCfg) {
                updateDisplayModeFromCurrentState();
                return;
            }
            m_model->setDisplayMode(mode);
            int posX = 0;

            for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
                switch (mode) {
                case EXTEND_MODE: {
                    auto *cfgHead = opCfg->enableHead(it.value());
                    if (!cfgHead) {
                        opCfg->deleteLater();
                        updateDisplayModeFromCurrentState();
                        return;
                    }
                    cfgHead->setPosition({ posX, 0 });
                    posX += it.key()->w();
                    break;
                }
                case SINGLE_MODE: {
                    if (it.key()->name() == name) {
                        auto *cfgHead = opCfg->enableHead(it.value());
                        if (!cfgHead) {
                            opCfg->deleteLater();
                            updateDisplayModeFromCurrentState();
                            return;
                        }
                        WQt::OutputMode *preferMode = nullptr;
                        for (auto *mode : it.value()->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
                            preferMode = mode;
                            if (mode->isPreferred())
                                break;
                        }
                        if (preferMode)
                            cfgHead->setMode(preferMode);
                        cfgHead->setPosition({ 0, 0 });
                    } else {
                        opCfg->disableHead(it.value());
                    }
                    break;
                }
                default:
                    break;
                }
            }

            opCfg->apply();
        }
    } else {
        m_displayInter->SwitchMode(static_cast<uchar>(mode), name).waitForFinished();
    }
}

void DisplayWorker::onMonitorListChanged(const QList<QDBusObjectPath> &mons)
{
    QList<QString> ops;
    for (const auto *mon : m_monitors.keys())
        ops << mon->path();

    qCDebug(DdcDisplayWorker) << mons.size();
    QList<QString> pathList;
    for (const auto &op : mons) {
        const QString path = op.path();
        pathList << path;
        if (!ops.contains(path))
            monitorAdded(path);
    }

    for (const auto &op : ops)
        if (!pathList.contains(op))
            monitorRemoved(op);
}

void DisplayWorker::onWlMonitorListChanged()
{
    // Only check new output here, listen OutputHead::finished for remove
    auto heads = m_reg->outputManager()->heads();

    qCDebug(DdcDisplayWorker) << heads.size();
    for (auto *head : heads) {
        bool isNew = true;
        for (const auto *oldHead : m_wl_monitors.values())
            if (head == oldHead) {
                isNew = false;
                break;
            }
        if (isNew)
            wlMonitorAdded(head);
    }
    for (auto output : m_reg->waylandOutputs()) {
        wlOutputAdded(output);
    }
    connect(m_reg, &WQt::Registry::outputAdded, this, &DisplayWorker::wlOutputAdded);
    connect(m_reg, &WQt::Registry::outputRemoved, this, &DisplayWorker::wlOutputRemoved);
}

void DisplayWorker::updateWallpaper()
{
    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it) {
        updateMonitorWallpaper(it.key());
    }
}

void DisplayWorker::updateMonitorWallpaper(Monitor *mon)
{
    QString wallpaper = m_displayInter->GetCurrentWorkspaceBackgroundForMonitor(mon->name());
    if (isVideoFile(wallpaper)) {
        wallpaper = resolveVideoThumbnail(wallpaper, mon);
    }
    mon->setWallpaper(wallpaper);
}

void DisplayWorker::updateWallpaperFromWayland()
{
    auto *wpMgr = m_reg->wallpaperManager();
    if (!wpMgr || !wpMgr->isActive())
        return;

    for (auto *output : m_reg->waylandOutputs()) {
        if (!output || !output->get())
            continue;

        auto *wp = wpMgr->getWallpaper(output->get());
        if (!wp)
            continue;
        connect(wp, &WQt::Wallpaper::changed, this, &DisplayWorker::onWallpaperChanged, Qt::UniqueConnection);
    }
}

void DisplayWorker::onOutputWallpaperReady(WQt::Output *output)
{
    auto *wpMgr = m_reg->wallpaperManager();
    if (!output || !output->get() || !wpMgr || !wpMgr->isActive())
        return;

    auto *wp = wpMgr->getWallpaper(output->get());
    if (wp) {
        connect(wp, &WQt::Wallpaper::changed, this, &DisplayWorker::onWallpaperChanged, Qt::UniqueConnection);

        if (wp->sourceType() == WallpaperSourceTypeVideo && !wp->fileSource().isEmpty()) {
            for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
                if (it.key()->name() == output->name()) {
                    it.key()->setWallpaper(resolveVideoThumbnail(wp->fileSource(), it.key()));
                    break;
                }
            }
        }
    }
}

void DisplayWorker::onWallpaperChanged(const QString &fileSource, uint32_t sourceType, uint32_t role)
{
    Q_UNUSED(role);
    auto *wp = qobject_cast<WQt::Wallpaper *>(sender());
    if (!wp || !wp->output() || !m_reg)
        return;

    QString outputName;
    for (auto *output : m_reg->waylandOutputs()) {
        if (output && output->get() == wp->output()) {
            outputName = output->name();
            break;
        }
    }

    if (outputName.isEmpty())
        return;

    for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
        if (it.key()->name() == outputName) {
            QString wallpaper = fileSource;
            if (sourceType == WallpaperSourceTypeVideo) {
                wallpaper = resolveVideoThumbnail(fileSource, it.key());
            }
            it.key()->setWallpaper(wallpaper);
            break;
        }
    }
}

void DisplayWorker::updateVirtualOutputs()
{
    auto *voMgr = m_reg->virtualOutputManager();
    if (!voMgr)
        return;

    QMap<QString, QStringList> virtualOutputMap;
    for (auto it = voMgr->virtualOutputs().cbegin(); it != voMgr->virtualOutputs().cend(); ++it) {
        virtualOutputMap.insert(it.key(), it.value()->outputs());
    }
    m_model->setVirtualOutput(virtualOutputMap);
}

void DisplayWorker::updateTreelandDisplayMode()
{
    auto *voMgr = m_reg ? m_reg->virtualOutputManager() : nullptr;
    if (voMgr && !voMgr->virtualOutputs().isEmpty()) {
        m_model->setDisplayMode(MERGE_MODE);
        return;
    }

    int enabledCount = 0;
    for (auto *monitor : m_model->monitorList()) {
        if (monitor->enable())
            ++enabledCount;
    }

    m_model->setDisplayMode(enabledCount == 1 && m_model->monitorList().size() > 1 ? SINGLE_MODE : EXTEND_MODE);
}

void DisplayWorker::onBrightnessChanged(WQt::ColorControl *colorControl, double brightness)
{
    brightness = qBound(0.0, brightness / 100.0, 1.0);
    for (auto it(m_control_monitors.cbegin()); it != m_control_monitors.cend(); ++it) {
        if (it.value() == colorControl) {
            it.key()->setBrightness(brightness);
            return;
        }
    }
}

void DisplayWorker::onMonitorsBrightnessChanged(const BrightnessMap &brightness)
{
    if (brightness.isEmpty())
        return;

    for (auto it = m_monitors.begin(); it != m_monitors.end(); ++it) {
        it.key()->setBrightness(brightness[it.key()->name()]);
    }
}

void DisplayWorker::onGetScaleFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<double> reply = w->reply();

    m_model->setUIScale(reply);

    w->deleteLater();
}

void DisplayWorker::onGetScreenScalesFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QMap<QString, double>> reply = w->reply();
    QMap<QString, double> rmap = reply;

    for (auto &m : m_model->monitorList()) {
        if (rmap.find(m->name()) != rmap.end()) {
            m->setScale(rmap.value(m->name()) < 1.0 ? m_model->uiScale() : rmap.value(m->name()));
        }
    }

    w->deleteLater();
}

void DisplayWorker::onInterfaceRegistered(WQt::Registry::Interface interface)
{
    switch (interface) {
    case WQt::Registry::OutputManagerInterface: {
        m_model->setResolutionRefreshEnable(true);
        m_model->setBrightnessEnable(false);

        m_model->setColorTemperatureEnabled(true);
        m_model->setAdjustCCTmode(0);
        m_model->setColorTemperature(toColorTempPos(m_defaultMode));
        m_model->setRedshiftIsValid(true);
        auto *opMgr = m_reg->outputManager();
        if (!opMgr) {
            qCCritical(DdcDisplayWorker) << "Unable to start the output manager";
        } else {
            connect(opMgr, &WQt::OutputManager::done, this, &DisplayWorker::onWlOutputManagerDone);
        }
    } break;
    case WQt::Registry::TreeLandOutputManagerInterface: {
        updateControl();
    } break;
    case WQt::Registry::VirtualOutputManagerInterface: {
        auto *virtOpMgr = m_reg->virtualOutputManager();
        if (!virtOpMgr) {
            qCCritical(DdcDisplayWorker) << "Unable to start the virtual output manager";
        } else {
            virtOpMgr->getVirtualOutputList();
            connect(virtOpMgr, &WQt::VirtualOutputManager::virtualOutputList, this, [this, virtOpMgr](const QStringList &names) {
                for (const auto &name : names) {
                    auto *vo = virtOpMgr->virtualOutputs().value(name);
                    if (vo) {
                        connect(vo, &WQt::VirtualOutput::outputsChanged, this, [this]() {
                            updateVirtualOutputs();
                            updateTreelandDisplayMode();
                        });
                    }
                }
                updateVirtualOutputs();
                updateTreelandDisplayMode();
            });
        }
    } break;
    case WQt::Registry::WallpaperManagerInterface: {
        auto *wpMgr = m_reg->wallpaperManager();
        if (!wpMgr) {
            qCCritical(DdcDisplayWorker) << "Unable to start the wallpaper manager";
        } else {
            updateWallpaperFromWayland();
        }
    } break;
    default:
        break;
    }
}

void DisplayWorker::onWlOutputManagerDone()
{
    onWlMonitorListChanged();

    updateTreelandDisplayMode();
    auto *treelandOpMgr = m_reg->treeLandOutputManager();
    if (treelandOpMgr) {
        m_model->setPrimary(treelandOpMgr->mPrimaryOutput);
        connect(treelandOpMgr, &WQt::TreeLandOutputManager::primaryOutputChanged, this, [this]() {
            m_model->setPrimary(m_reg->treeLandOutputManager()->mPrimaryOutput);
        });
    }
}

uint32_t DisplayWorker::toColorTemp(int pos)
{
    int kelvin = pos > 50 ? (m_defaultMode - (m_defaultMode - m_tcMinValue) * (pos - 50) * 0.02) : (m_defaultMode + (m_tcMaxValue - m_defaultMode) * (50 - pos) * 0.02);
    if (kelvin < 1000)
        kelvin = 1000;
    else if (kelvin > 20000)
        kelvin = 20000;
    return static_cast<uint32_t>(kelvin);
}

int DisplayWorker::toColorTempPos(uint32_t temperature)
{
    int kelvin = static_cast<int>(temperature);
    if ((m_tcMaxValue <= m_defaultMode && kelvin >= m_defaultMode ) || (m_defaultMode <= m_tcMinValue && kelvin <= m_defaultMode)) {
        qCWarning(DdcDisplayWorker) << "Invalid color temperature range, returning mid-point";
        return 50;
    }
    int pos = kelvin >= m_defaultMode ? (m_tcMaxValue - kelvin) * 50.0 / (m_tcMaxValue - m_defaultMode) : 50 + (m_defaultMode - kelvin) * 50.0 / (m_defaultMode - m_tcMinValue);
    if (pos < 0) {
        pos = 0;
    } else if (pos > 100) {
        pos = 100;
    }
    return pos;
}
void DisplayWorker::initCTMData()
{
    QFile file(EyeProtectionConfig);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(DdcDisplayWorker) << "Config file is not exist: " << EyeProtectionConfig;
        return;
    }
    QXmlStreamReader xml(&file);
    QVector<int> val;
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            // 获取PanelCCT和RgbGammaLut的值
            if (xml.name() == "param") {
                QString paramName = xml.attributes().value("name").toString();
                QString paramValue = xml.readElementText();
                if (paramName == "PanelCCT") {
                    val.push_back(paramValue.toInt());
                }
            }
        }
    }
    // 关闭文件
    file.close();
    if (val.isEmpty()) {
        qCWarning(DdcDisplayWorker) << "No PanelCCT param found in config:" << EyeProtectionConfig;
        return;
    }
    std::sort(val.begin(), val.end());
    m_tcMinValue = val.first();
    m_tcMaxValue = val.last();
    m_defaultMode = 4500;
}
#ifndef DCC_DISABLE_ROTATE

constexpr static int wlRotate2dcc(int wlRotate)
{
    switch (wlRotate) {
    case WL_OUTPUT_TRANSFORM_NORMAL:
        return 1;
    case WL_OUTPUT_TRANSFORM_90:
        return 2;
    case WL_OUTPUT_TRANSFORM_180:
        return 4;
    case WL_OUTPUT_TRANSFORM_270:
        return 8;
    default:
        qWarning("dcc dont support FLIPPED");
        return 0;
    }
}

constexpr static int dccRotate2wl(int dccRotate)
{
    switch (dccRotate) {
    case 1:
        return WL_OUTPUT_TRANSFORM_NORMAL;
    case 2:
        return WL_OUTPUT_TRANSFORM_90;
    case 4:
        return WL_OUTPUT_TRANSFORM_180;
    case 8:
        return WL_OUTPUT_TRANSFORM_270;
    default:
        qWarning("unkone dccRotate, feedback to normal");
        return WL_OUTPUT_TRANSFORM_NORMAL;
    }
}

void DisplayWorker::setMonitorRotate(Monitor *mon, const quint16 rotate)
{
    m_model->setmodeChanging(true);
    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();
        for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
            if (!it.key()->enable()) {
                opCfg->disableHead(it.value());
                continue;
            }
            auto *cfgHead = opCfg->enableHead(it.value());
            if (m_model->displayMode() == MERGE_MODE || it.key() == mon) {
                cfgHead->setTransform(dccRotate2wl(rotate));
            }
        }
        opCfg->apply();
    } else {
        if (m_model->displayMode() == MERGE_MODE) {
            for (auto *m : m_monitors) {
                m->SetRotation(rotate).waitForFinished();
            }
        } else {
            MonitorDBusProxy *inter = m_monitors.value(mon);
            inter->SetRotation(rotate).waitForFinished();
        }
    }
}
#endif

void DisplayWorker::setPrimary(const QString &name)
{
    if (WQt::Utils::isTreeland()) {
        auto *treelandOpMgr = m_reg->treeLandOutputManager();
        if (treelandOpMgr)
            treelandOpMgr->setPrimaryOutput(name.toStdString().c_str());
    } else {
        m_displayInter->SetPrimary(name);
    }
}

void DisplayWorker::setMonitorEnable(Monitor *monitor, const bool enable)
{
    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();

        for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
            if (it.key() == monitor) {
                if (enable) {
                    auto *cfgHead = opCfg->enableHead(it.value());
                    WQt::OutputMode *preferMode = nullptr;
                    for (auto *mode : it.value()->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
                        preferMode = mode;
                        if (mode->isPreferred())
                            break;
                    }
                    cfgHead->setMode(preferMode);
                } else {
                    opCfg->disableHead(it.value());
                }
            } else {
                if (!it.key()->enable()) {
                    opCfg->disableHead(it.value());
                } else {
                    opCfg->enableHead(it.value());
                }
            }
        }
        opCfg->apply();
    } else {
        MonitorDBusProxy *inter = m_monitors.value(monitor);
        inter->Enable(enable).waitForFinished();
        applyChanges();
    }
}

void DisplayWorker::applyChanges()
{
    if (!m_timer->isActive()) {
        m_timer->start();
    }
}

void DisplayWorker::setColorTemperatureEnabled(bool enabled)
{
    if (WQt::Utils::isTreeland()) {
        uint32_t temp = enabled ? toColorTemp(m_model->colorTemperature()) : toColorTemp(m_defaultMode);
        for (auto it(m_control_monitors.cbegin()); it != m_control_monitors.cend(); ++it) {
            if (it.value())
                it.value()->setColorTemperature(temp);
        }
    } else {
        m_displayInter->setColorTemperatureEnabled(enabled);
    }
}

void DisplayWorker::setColorTemperature(int pos)
{
    if (WQt::Utils::isTreeland()) {
        uint32_t temp = toColorTemp(pos);
        for (auto it(m_control_monitors.cbegin()); it != m_control_monitors.cend(); ++it) {
            if (it.value()) {
                it.value()->setColorTemperature(temp);
            }
        }
    } else {
        m_displayInter->SetColorTemperature(toColorTemp(pos)).waitForFinished();
    }
}

void DisplayWorker::SetMethodAdjustCCT(int mode)
{
    if (WQt::Utils::isTreeland()) {
        qCDebug(DdcDisplayWorker) << "==treeland==" << "SetMethodAdjustCCT:" << mode;
    } else {
        m_displayInter->SetMethodAdjustCCT(mode);
    }
}

void DisplayWorker::setCustomColorTempTimePeriod(const QString &timePeriod)
{
    m_displayInter->SetCustomColorTempTimePeriod(timePeriod);
}

void DisplayWorker::setCurrentFillMode(Monitor *mon, const QString fillMode)
{
    if (WQt::Utils::isTreeland()) {
        // TODO: support treeland
    } else {
        MonitorDBusProxy *inter = m_monitors.value(mon);
        Q_ASSERT(inter);
        inter->setCurrentFillMode(fillMode);
    }
}

void DisplayWorker::backupConfig()
{
    m_displayConfig = m_displayInter->GetConfig();
}

void DisplayWorker::clearBackup()
{
    m_displayConfig.clear();
}

void DisplayWorker::resetBackup()
{
    // TODO: can't use in treeland
    if (!m_displayConfig.isEmpty()) {

        QJsonDocument doc = QJsonDocument::fromJson(m_displayConfig.toLatin1());
        QJsonObject jsonObj = doc.object();

        QDateTime time = QDateTime::currentDateTime();
        int offset = time.offsetFromUtc() / 60;
        bool negative = offset < 0;
        if (negative)
            offset = -offset;

        jsonObj.insert("UpdateAt", QString("%1%2%3:%4").arg(time.toString("yyyy-MM-ddThh:mm:ss.zzz000000")).arg(negative ? '-' : '+').arg(offset / 60, 2, 10, QChar('0')).arg(offset % 60, 2, 10, QChar('0')));
        doc.setObject(jsonObj);

        m_displayInter->SetConfig(doc.toJson(QJsonDocument::Compact));
        clearBackup();
    }
}

void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    m_model->setmodeChanging(true);
    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();
        auto res = mon->getResolutionById(mode);
        if (!res.has_value())
            return;

        for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
            if (!it.key()->enable()) {
                opCfg->disableHead(it.value());
                continue;
            }
            auto *cfgHead = opCfg->enableHead(it.value());
            if (it.key() == mon) {
                for (auto *mode : it.value()->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
                    if (mode->size().width() == res.value().width() && mode->size().height() == res.value().height() && qFuzzyCompare(mode->refreshRate() * 0.001, res.value().rate())) {
                        cfgHead->setMode(mode);
                        break;
                    }
                }
            }
        }
        opCfg->apply();
    } else {
        MonitorDBusProxy *inter = m_monitors.value(mon);
        if (inter)
            inter->SetMode(static_cast<uint>(mode)).waitForFinished();
    }
}

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    if (WQt::Utils::isTreeland()) {
#if GAMMA_SUPPORT
        auto *gammaEffect = m_wl_gammaEffects->value(mon);
        auto *gammaConfig = m_wl_gammaConfig->value(mon);
        gammaConfig->brightness = brightness;
        gammaEffect->setConfiguration(*gammaConfig);
#else
        for (auto it(m_control_monitors.cbegin()); it != m_control_monitors.cend(); ++it) {
            if (it.key() == mon) {
                it.value()->setBrightness(brightness * 100.0);
                break;
            }
        }
#endif
    } else {
        m_displayInter->SetAndSaveBrightness(mon->name(), std::max(brightness, m_model->minimumBrightnessScale())).waitForFinished();
    }
}

void DisplayWorker::updateMonitorPosition(const QHash<Monitor *, QPair<int, int>> &monitorPosition)
{
    for (auto it(monitorPosition.cbegin()); it != monitorPosition.cend(); ++it) {
        it.key()->setX(it.value().first);
        it.key()->setY(it.value().second);
    }
}

void DisplayWorker::setMonitorPosition(const QHash<Monitor *, QPair<int, int>> monitorPosition)
{
    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();
        for (auto it(monitorPosition.cbegin()); it != monitorPosition.cend(); ++it) {
            auto *head = m_wl_monitors.value(it.key());
            Q_ASSERT(head);
            if (!it.key()->enable()) {
                opCfg->disableHead(head);
                continue;
            }
            auto *cfgHead = opCfg->enableHead(head);
            cfgHead->setPosition({ it.value().first, it.value().second });
        }
        opCfg->apply();
    } else {
        for (auto it(monitorPosition.cbegin()); it != monitorPosition.cend(); ++it) {
            MonitorDBusProxy *inter = m_monitors.value(it.key());
            Q_ASSERT(inter);
            inter->SetPosition(static_cast<short>(it.value().first), static_cast<short>(it.value().second)).waitForFinished();
        }
        applyChanges();
    }
}

void DisplayWorker::setUiScale(const double value)
{
    qCDebug(DdcDisplayWorker) << "set display scale:" << value;
    double rv = value;
    if (rv < 0)
        rv = m_model->uiScale();
    for (auto &mm : m_model->monitorList()) {
        mm->setScale(-1);
    }

    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();
        for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
            if (!it.key()->enable()) {
                opCfg->disableHead(it.value());
                continue;
            }
            auto *cfgHead = opCfg->enableHead(it.value());
            cfgHead->setScale(rv);
        }
        opCfg->apply();
        connect(opCfg, &WQt::OutputConfiguration::succeeded, this, [this, rv]() {
            m_model->setUIScale(rv);
        });
    } else {
        QDBusPendingCall call = m_displayInter->SetScaleFactor(rv);

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        watcher->waitForFinished();
        if (!watcher->isError()) {
            m_model->setUIScale(rv);
        }
        watcher->deleteLater();
    }
}

void DisplayWorker::setIndividualScaling(Monitor *m, const double scaling)
{
    if (m && scaling >= 1.0) {
        m->setScale(scaling);
    } else {
        return;
    }

    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();
        for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
            if (!it.key()->enable()) {
                opCfg->disableHead(it.value());
                continue;
            }
            auto *cfgHead = opCfg->enableHead(it.value());
            if (it.key() == m) {
                cfgHead->setScale(scaling);
            }
        }
        opCfg->apply();
    } else {
        QMap<QString, double> scalemap;
        for (Monitor *m : m_model->monitorList()) {
            scalemap[m->name()] = m_model->monitorScale(m);
        }
        m_displayInter->SetScreenScaleFactors(scalemap);
    }
}

void DisplayWorker::monitorAdded(const QString &path)
{
    MonitorDBusProxy *inter = new MonitorDBusProxy(path, this);
    Monitor *mon = new Monitor(this);

    connect(inter, &MonitorDBusProxy::XChanged, mon, &Monitor::setX);
    connect(inter, &MonitorDBusProxy::YChanged, mon, &Monitor::setY);
    connect(inter, &MonitorDBusProxy::WidthChanged, mon, &Monitor::setW);
    connect(inter, &MonitorDBusProxy::HeightChanged, mon, &Monitor::setH);
    connect(inter, &MonitorDBusProxy::MmWidthChanged, mon, &Monitor::setMmWidth);
    connect(inter, &MonitorDBusProxy::MmHeightChanged, mon, &Monitor::setMmHeight);
    connect(inter, &MonitorDBusProxy::RotationChanged, mon, &Monitor::setRotate);
    connect(inter, &MonitorDBusProxy::NameChanged, mon, &Monitor::setName);
    connect(inter, &MonitorDBusProxy::CurrentModeChanged, mon, &Monitor::setCurrentMode);
    connect(inter, &MonitorDBusProxy::BestModeChanged, mon, &Monitor::setBestMode);
    connect(inter, &MonitorDBusProxy::CurrentModeChanged, this, [=](Resolution value) {
        if (value.id() == 0) {
            return;
        }
        auto maxWScale = value.width() / 1024.0;
        auto maxHScale = value.height() / 768.0;
        auto maxScale = maxWScale < maxHScale ? maxWScale : maxHScale;
        if ((m_model->uiScale() - maxScale) > 0.01 && maxScale >= 1.0) {
            m_currentScale = 1.0;
            for (int idx = 0; idx * 0.25 + 1.0 <= maxScale; ++idx) {
                m_currentScale = idx * 0.25 + 1.0;
            }
            m_updateScale = true;
        }
    });

    connect(inter, &MonitorDBusProxy::ModesChanged, mon, &Monitor::setModeList);
    connect(inter, &MonitorDBusProxy::RotationsChanged, mon, &Monitor::setRotateList);
    connect(inter, &MonitorDBusProxy::EnabledChanged, mon, &Monitor::setMonitorEnable);
    connect(inter, &MonitorDBusProxy::CurrentRotateModeChanged, mon, &Monitor::setCurrentRotateMode);
    connect(inter, &MonitorDBusProxy::AvailableFillModesChanged, mon, &Monitor::setAvailableFillModes);
    connect(inter, &MonitorDBusProxy::CurrentFillModeChanged, mon, &Monitor::setCurrentFillMode);
    connect(this, &DisplayWorker::requestUpdateModeList, this, [=] {
        mon->setModeList(inter->modes());
    });

    // NOTE: DO NOT using async dbus call. because we need to have a unique name to distinguish each monitor
    mon->setName(inter->name());
    mon->setManufacturer(inter->manufacturer());
    mon->setModel(inter->model());
    QDBusReply<bool> reply = m_displayInter->CanSetBrightnessSync(inter->name());
    mon->setCanBrightness(reply.value());
    mon->setMonitorEnable(inter->enabled());
    mon->setCurrentRotateMode(inter->currentRotateMode());
    mon->setMonitorEnable(inter->enabled());
    mon->setCurrentFillMode(inter->currentFillMode());
    mon->setAvailableFillModes(inter->availableFillModes());
    mon->setPath(path);
    mon->setX(inter->x());
    mon->setY(inter->y());
    mon->setW(inter->width());
    mon->setH(inter->height());
    mon->setRotate(inter->rotation());
    mon->setCurrentMode(inter->currentMode());
    mon->setBestMode(inter->bestMode());
    mon->setModeList(inter->modes());
    if (m_model->isRefreshRateEnable() == false) {
        for (auto resolutionModel : mon->modeList()) {
            if (qFuzzyCompare(resolutionModel.rate(), 0.0) == false) {
                m_model->setRefreshRateEnable(true);
            }
        }
    }
    mon->setRotateList(inter->rotations());
    mon->setMmWidth(inter->mmWidth());
    mon->setMmHeight(inter->mmHeight());

    if (!m_model->brightnessMap().isEmpty()) {
        mon->setBrightness(m_model->brightnessMap()[mon->name()]);
    }
    updateMonitorWallpaper(mon);
    m_model->monitorAdded(mon);
    m_monitors.insert(mon, inter);
}

void DisplayWorker::monitorRemoved(const QString &path)
{
    Monitor *monitor = nullptr;
    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it) {
        if (it.key()->path() == path) {
            monitor = it.key();
            break;
        }
    }
    if (!monitor)
        return;

    m_model->monitorRemoved(monitor);

    m_monitors[monitor]->deleteLater();
    m_monitors.remove(monitor);

    monitor->deleteLater();
}

static inline Resolution createResolutionFromMode(WQt::OutputMode *mode)
{
    static int idcount = 0;
    Resolution res;
    if (!mode) {
        res.m_id = 0;
        return res;
    }
    res.m_id = ++idcount;
    res.m_width = mode->size().width();
    res.m_height = mode->size().height();
    res.m_rate = mode->refreshRate() * 0.001;
    return res;
}

void DisplayWorker::wlMonitorAdded(WQt::OutputHead *head)
{
    Monitor *mon = new Monitor(this);

    connect(head, &WQt::OutputHead::finished, this, [head, this]() {
        wlMonitorRemoved(head);
    });

    connect(head, &WQt::OutputHead::changed, mon, [this, mon, head](WQt::OutputHead::Property type) {
        switch (type) {
        case WQt::OutputHead::Name:
            mon->setName(head->property(WQt::OutputHead::Name).toString());
            break;
        case WQt::OutputHead::PhysicalSize: {
            auto physicalSize = head->property(WQt::OutputHead::PhysicalSize).toSize();
            mon->setMmWidth(physicalSize.width());
            mon->setMmHeight(physicalSize.height());
            break;
        }
        case WQt::OutputHead::Modes: {
            ResolutionList resolutionList;
            for (auto *mode : head->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
                Resolution res = createResolutionFromMode(mode);
                resolutionList << res;
                if (mode->isPreferred()) {
                    mon->setBestMode(res);
                }
            }
            mon->setModeList(resolutionList);
            break;
        }
        case WQt::OutputHead::CurrentMode: {
            Resolution currentRes = createResolutionFromMode(head->property(WQt::OutputHead::CurrentMode).value<WQt::OutputMode *>());
            mon->setCurrentMode(currentRes);
            mon->setW(currentRes.width());
            mon->setH(currentRes.height());
            break;
        }
        case WQt::OutputHead::Position:
            mon->setX(head->property(WQt::OutputHead::Position).toPoint().x());
            mon->setY(head->property(WQt::OutputHead::Position).toPoint().y());
            break;
        case WQt::OutputHead::Transform:
            mon->setRotate(wlRotate2dcc(head->property(WQt::OutputHead::Transform).toInt()));
            break;
        case WQt::OutputHead::Scale:
            mon->setScale(head->property(WQt::OutputHead::Scale).toFloat());
            break;
        case WQt::OutputHead::Enabled:
            mon->setMonitorEnable(head->property(WQt::OutputHead::Enabled).toBool());
            updateTreelandDisplayMode();
            break;
        case WQt::OutputHead::Make:
            mon->setManufacturer(head->property(WQt::OutputHead::Make).toString());
            break;
        case WQt::OutputHead::Model:
            mon->setModel(head->property(WQt::OutputHead::Model).toString());
            break;
        case WQt::OutputHead::Description:
        case WQt::OutputHead::SerialNumber:
            // Not handle
        default:
            break;
        }
    });

    // TODO: where to get UI Scale for model
    m_model->setUIScale(head->property(WQt::OutputHead::Scale).toFloat());
    mon->setScale(head->property(WQt::OutputHead::Scale).toFloat());

    // NOTE: we need to have a unique name to distinguish each monitor
    mon->setName(head->property(WQt::OutputHead::Name).toString());
    mon->setManufacturer(head->property(WQt::OutputHead::Make).toString());
    mon->setModel(head->property(WQt::OutputHead::Model).toString());
    mon->setMonitorEnable(head->property(WQt::OutputHead::Enabled).toBool());
    mon->setCanBrightness(true);

    mon->setX(head->property(WQt::OutputHead::Position).toPoint().x());
    mon->setY(head->property(WQt::OutputHead::Position).toPoint().y());

    mon->setRotateList({ 1, 2, 4, 8 });
    mon->setRotate(wlRotate2dcc(head->property(WQt::OutputHead::Transform).toInt()));

    ResolutionList resolutionList;
    for (auto *mode : head->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
        Resolution res = createResolutionFromMode(mode);
        resolutionList << res;
        if (mode->isPreferred()) {
            mon->setBestMode(res);
        }
    }
    mon->setModeList(resolutionList);
    Resolution currentRes = createResolutionFromMode(head->property(WQt::OutputHead::CurrentMode).value<WQt::OutputMode *>());
    if (currentRes.id() != 0) { // 0 is invalid
        mon->setCurrentMode(currentRes);
        mon->setW(currentRes.width());
        mon->setH(currentRes.height());
    }

    if (m_model->isRefreshRateEnable() == false) {
        for (auto resolutionModel : mon->modeList()) {
            if (qFuzzyCompare(resolutionModel.rate(), 0.0) == false) {
                m_model->setRefreshRateEnable(true);
            }
        }
    }

    auto physicalSize = head->property(WQt::OutputHead::PhysicalSize).toSize();
    mon->setMmWidth(physicalSize.width());
    mon->setMmHeight(physicalSize.height());

    m_model->monitorAdded(mon);
    m_wl_monitors.insert(mon, head);
    updateControl();

#if GAMMA_SUPPORT
    auto *gammaMgr = m_reg->gammaControlManager();
    auto *gammaEffect = new DFL::GammaEffects(gammaMgr->getGammaControl(op->get()));
    auto *effectsConfig = new DFL::GammaEffectsConfig;
    effectsConfig->mode = 0x8EC945;
    effectsConfig->gamma = 0.5;
    effectsConfig->brightness = 0.3;
    effectsConfig->minTemp = 4000;
    effectsConfig->maxTemp = 6500;
    effectsConfig->temperature = 6500;
    effectsConfig->latitude = 0; // How to get
    effectsConfig->longitude = 0;
    effectsConfig->sunrise = QTime(6, 30, 0);
    effectsConfig->sunset = QTime(18, 30, 0);
    effectsConfig->whitepoint = { 0, 0, 0 };

    // gammaEffect->setConfiguration(config);
    m_wl_gammaEffects->insert(mon, gammaEffect);
    m_wl_gammaConfig->insert(mon, effectsConfig);
#endif
}

void DisplayWorker::wlMonitorRemoved(WQt::OutputHead *head)
{
    Monitor *monitor = nullptr;
    for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
        if (it.value() == head) {
            monitor = it.key();
            break;
        }
    }
    if (!monitor)
        return;

    m_model->monitorRemoved(monitor);

#if GAMMA_SUPPORT
    // delete m_wl_gammaConfig[monitor];
    // delete m_wl_gammaEffects[monitor];
#endif
    head->deleteLater();

    m_wl_monitors.remove(monitor);
    if (m_control_monitors.contains(monitor)) {
        delete m_control_monitors.value(monitor);
        m_control_monitors.remove(monitor);
    }

    monitor->deleteLater();
}

void DisplayWorker::wlOutputAdded(WQt::Output *output)
{
    if (!output)
        return;

    connect(output, &WQt::Output::done, this, &DisplayWorker::updateControl);
    connect(output, &WQt::Output::done, this, [this, output]() {
        onOutputWallpaperReady(output);
    });
}

void DisplayWorker::wlOutputRemoved(WQt::Output *output)
{
    auto *wpMgr = m_reg ? m_reg->wallpaperManager() : nullptr;
    if (output && wpMgr)
        wpMgr->removeWallpaper(output->get());
}

void DisplayWorker::updateControl()
{
    auto *treelandOpMgr = m_reg->treeLandOutputManager();
    if (!treelandOpMgr) {
        return;
    }
    for (auto output : m_reg->waylandOutputs()) {
        if (output->isReady()) {
            for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
                if (it.key()->name() == output->name()) {
                    if (!m_control_monitors.contains(it.key())) {
                        auto control = treelandOpMgr->getColorControl(output->get());
                        if (control) {
                            connect(control, &WQt::ColorControl::brightnessChanged, this, [this, control](double brightness) {
                                onBrightnessChanged(control, brightness);
                            });
                            connect(control, &WQt::ColorControl::colorTemperatureChanged, this, [this](uint32_t temperature) {
                                m_model->setColorTemperature(toColorTempPos(temperature));
                            });
                        }
                        m_control_monitors.insert(it.key(), control);
                    }
                    break;
                }
            }
        }
    }
}

void DisplayWorker::setAmbientLightAdjustBrightness(bool able)
{
    m_displayInter->setAmbientLightAdjustBrightness(able);
}

void DisplayWorker::initAutoBacklight()
{
    if (!m_displayInter)
        return;

    // 读取自动亮度支持状态
    bool supported = m_displayInter->autoBrightnessSupported();
    m_model->setAutoBacklightSupported(supported);

    // 读取自动亮度启用状态
    bool enabled = m_displayInter->autoBrightnessEnabled();
    m_model->setAutoBacklightEnabled(enabled);

    // 获取内置屏幕名称
    QString builtinName = m_displayInter->GetBuiltinMonitorName();
    if (!builtinName.isEmpty()) {
        m_model->setBuiltinMonitorName(builtinName);
    }

    // 连接信号
    connect(m_displayInter, &DisplayDBusProxy::AutoBrightnessSupportedChanged, m_model, &DisplayModel::setAutoBacklightSupported);
    connect(m_displayInter, &DisplayDBusProxy::AutoBrightnessEnabledChanged, m_model, &DisplayModel::setAutoBacklightEnabled);
    connect(m_model, &DisplayModel::requestSetAutoBacklightEnable, this, &DisplayWorker::setAutoBacklightEnabled);
}

void DisplayWorker::setAutoBacklightEnabled(const bool value)
{
    if (!m_displayInter)
        return;

    QDBusPendingReply<> reply = m_displayInter->SetAutoBrightnessEnabled(value);
    reply.waitForFinished();
    if (reply.isError()) {
        qCWarning(DdcDisplayWorker) << "Set auto brightness enabled failed, error: " << reply.error().message();
    } else {
        qCInfo(DdcDisplayWorker) << "Set `AutoBrightnessEnabled`, value: " << value;
    }
}

void DisplayWorker::setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID)
{
    m_displayInter->AssociateTouch(monitor, touchscreenUUID);
}

void DisplayWorker::setMonitorResolutionBySize(Monitor *mon, const int width, const int height)
{
    m_model->setmodeChanging(true);
    if (WQt::Utils::isTreeland()) {
        auto *opCfg = m_reg->outputManager()->createConfiguration();
        for (auto it(m_wl_monitors.cbegin()); it != m_wl_monitors.cend(); ++it) {
            if (!it.key()->enable()) {
                opCfg->disableHead(it.value());
                continue;
            }
            auto *cfgHead = opCfg->enableHead(it.value());
            if (it.key() == mon)
                cfgHead->setCustomMode({ width, height }, mon->currentMode().rate());
        }
        opCfg->apply();
    } else {
        MonitorDBusProxy *inter = m_monitors.value(mon);
        Q_ASSERT(inter);

        QDBusPendingCall call = inter->SetModeBySize(static_cast<ushort>(width), static_cast<ushort>(height));
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (call.isError()) {
                qCDebug(DdcDisplayWorker) << call.error().message();
            }
            watcher->deleteLater();
        });
        watcher->waitForFinished();
    }
}

void DisplayWorker::setConcatScreenMode(bool enable)
{
    if (WQt::Utils::isTreeland()) {
        return;
    }

    qCDebug(DdcDisplayWorker) << "[ConcatScreen] setConcatScreenMode" << enable;
    QDBusPendingReply<> reply = m_displayInter->SetConcatScreen(enable);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, enable](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<> r = *w;
        if (r.isError()) {
            qCWarning(DdcDisplayWorker) << "[ConcatScreen] setConcatScreenMode failed:" << r.error().message();
            m_model->setIsConcatScreenMode(m_displayInter->isConcatScreenEnabled());
        } else {
            m_model->setIsConcatScreenMode(enable);
        }
        w->deleteLater();
    });
}

void DisplayWorker::updateConcatScreenMode()
{
    if (WQt::Utils::isTreeland()) {
        return;
    }

    qCDebug(DdcDisplayWorker) << "[ConcatScreen] updateConcatScreenMode via DBus property";
    m_model->setIsConcatScreenMode(m_displayInter->isConcatScreenEnabled());
}

QString DisplayWorker::resolveVideoThumbnail(const QString &videoPath, Monitor *monitor)
{
    QDir liveDir(SysLiveWallpaperDir);
    if (liveDir.exists()) {
        QFile metaFile(liveDir.absoluteFilePath("metadata.json"));
        if (metaFile.open(QIODevice::ReadOnly)) {
            QJsonParseError parseErr;
            QJsonDocument doc = QJsonDocument::fromJson(metaFile.readAll(), &parseErr);
            metaFile.close();
            if (parseErr.error != QJsonParseError::NoError || !doc.isArray()) {
                qCWarning(DdcDisplayWorker) << "metadata.json parse error:" << parseErr.errorString();
            } else {
                for (const auto &entry : doc.array()) {
                    QJsonObject obj = entry.toObject();
                    QString videoAbsPath = liveDir.absoluteFilePath(obj.value("path").toString());
                    if (videoAbsPath == videoPath) {
                        QString thumbnailRel = obj.value("thumbnail").toString();
                        if (!thumbnailRel.isEmpty()) {
                            QString thumbAbsPath = liveDir.absoluteFilePath(thumbnailRel);
                            if (QFile::exists(thumbAbsPath)) {
                                return thumbAbsPath;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/live-wallpaper-thumbnails";
    if (cacheDir.isEmpty() || !QDir().mkpath(cacheDir)) {
        qCWarning(DdcDisplayWorker) << "Failed to create thumbnail cache directory:" << cacheDir;
        return videoPath;
    }
    QString cacheName = QString(QCryptographicHash::hash(videoPath.toUtf8(), QCryptographicHash::Md5).toHex()) + ".png";
    QString cachePath = cacheDir + "/" + cacheName;

    if (QFile::exists(cachePath)) {
        return cachePath;
    }

    if (m_videoWallpaperMonitors.contains(videoPath)) {
        m_videoWallpaperMonitors[videoPath].append(monitor);
        return videoPath;
    }

    m_videoWallpaperMonitors[videoPath] = { monitor };

    QPointer<DisplayWorker> guard(this);
    (void)QtConcurrent::run([guard, videoPath, cachePath]() {
        ffmpegthumbnailer::VideoThumbnailer thumbnailer(480, false, true, 8, false);
        thumbnailer.setThumbnailSize(480, -1);
        thumbnailer.setSeekTime("00:00:01");

        try {
            thumbnailer.generateThumbnail(videoPath.toStdString(), Png, cachePath.toStdString());
        } catch (const std::exception &e) {
            qCWarning(DdcDisplayWorker) << "Failed to generate video thumbnail:" << e.what();
            if (guard) {
                Q_EMIT guard->videoThumbnailReady(videoPath, {});
            }
            return;
        }

        if (guard && QFile::exists(cachePath)) {
            Q_EMIT guard->videoThumbnailReady(videoPath, cachePath);
        }
    });

    return videoPath;
}
