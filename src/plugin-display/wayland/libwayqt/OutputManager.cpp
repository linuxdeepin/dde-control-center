// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OutputManager.h"

#include "WayQtLogging.h"

#include <wayland-client.h>

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QLoggingCategory>
#include <QObject>
#include <QThread>
#include <QVariant>

/**
 * Output Manager
 * This is the beginning of all our operations.
 * Obtain a pointer to this object from Wayland Registry
 */

WQt::OutputManager::OutputManager(QObject *parent)
    : QWaylandClientExtensionTemplate<WQt::OutputManager>(2)
{
    setParent(parent);
}

WQt::OutputManager::~OutputManager() { }

QList<WQt::OutputHead *> WQt::OutputManager::heads()
{
    return mHeads;
}

WQt::OutputConfiguration *WQt::OutputManager::createConfiguration()
{
    qCDebug(DccWayQt) << "OutputManager::createConfiguration serial:" << mSerial;
    return new WQt::OutputConfiguration(QtWayland::zwlr_output_manager_v1::create_configuration(mSerial), this);
}

void WQt::OutputManager::stop()
{
    qCDebug(DccWayQt) << "OutputManager::stop";
    QtWayland::zwlr_output_manager_v1::stop();
}

::zwlr_output_manager_v1 *WQt::OutputManager::get()
{
    return object();
}

void WQt::OutputManager::zwlr_output_manager_v1_head(zwlr_output_head_v1 *id)
{
    WQt::OutputHead *opHead = new WQt::OutputHead(id, this);

    mHeads << opHead;
    qCInfo(DccWayQt) << "OutputHead added:" << opHead << "total:" << mHeads.count();
    connect(opHead, &WQt::OutputHead::finished, [=]() {
        mHeads.removeAll(opHead);
    });

    Q_EMIT headAttached(opHead);
}

void WQt::OutputManager::zwlr_output_manager_v1_done(uint32_t serial)
{
    mSerial = serial;
    mIsDone = true;
    qCDebug(DccWayQt) << "OutputManager::done serial:" << serial;

    Q_EMIT done();
}

void WQt::OutputManager::zwlr_output_manager_v1_finished()
{
    qCDebug(DccWayQt) << "OutputManager::finished";
}

/**
 * Output Head
 * Heads are obtained from OutputManager class, via signals,
 * or via OutputManager::heads() function;
 */

WQt::OutputHead::OutputHead(QObject *parent)
    : QObject(parent)
{
}

WQt::OutputHead::OutputHead(zwlr_output_head_v1 *head, QObject *parent)
    : QObject(parent)
    , mObj(head)
{
    setupListeners();
}

WQt::OutputHead::OutputHead(const WQt::OutputHead &otherHead)
    : QObject()
{
    mObj = otherHead.mObj;

    mPropsMap = otherHead.mPropsMap;
    mModes = otherHead.mModes;
    mCurrentMode = otherHead.mCurrentMode;
}

WQt::OutputHead::~OutputHead()
{
    if (mObj)
        zwlr_output_head_v1_destroy(mObj);
}

QVariant WQt::OutputHead::property(WQt::OutputHead::Property prop)
{
    if (prop == WQt::OutputHead::Modes) {
        return QVariant::fromValue<QList<WQt::OutputMode *>>(mModes);
    }

    else if (prop == WQt::OutputHead::CurrentMode) {
        return QVariant::fromValue<WQt::OutputMode *>(mCurrentMode);
    }

    else {
        return mPropsMap.value((int)prop);
    }
}

zwlr_output_head_v1 *WQt::OutputHead::get()
{
    return mObj;
}

void WQt::OutputHead::setupListeners()
{
    static const zwlr_output_head_v1_listener listener = {
        &WQt::OutputHead::handleName,      &WQt::OutputHead::handleDescription, &WQt::OutputHead::handlePhysicalSize, &WQt::OutputHead::handleMode, &WQt::OutputHead::handleEnabled, &WQt::OutputHead::handleCurrentMode,  &WQt::OutputHead::handlePosition,
        &WQt::OutputHead::handleTransform, &WQt::OutputHead::handleScale,       &WQt::OutputHead::handleFinished,     &WQt::OutputHead::handleMake, &WQt::OutputHead::handleModel,   &WQt::OutputHead::handleSerialNumber, &WQt::OutputHead::handleAdaptiveSync,
    };
    zwlr_output_head_v1_add_listener(mObj, &listener, this);
}

void WQt::OutputHead::handleName(void *data, zwlr_output_head_v1 *, const char *name)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Name] = QString::fromUtf8(name);
    Q_EMIT opHead->changed(WQt::OutputHead::Name);
}

void WQt::OutputHead::handleDescription(void *data, zwlr_output_head_v1 *, const char *descr)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Description] = QString::fromUtf8(descr);
    Q_EMIT opHead->changed(WQt::OutputHead::Description);
}

void WQt::OutputHead::handlePhysicalSize(void *data, zwlr_output_head_v1 *, int32_t width, int32_t height)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::PhysicalSize] = QSize(width, height);
    Q_EMIT opHead->changed(WQt::OutputHead::PhysicalSize);
}

void WQt::OutputHead::handleMode(void *data, zwlr_output_head_v1 *, zwlr_output_mode_v1 *mode)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    if (opHead->mPropsMap.contains(WQt::OutputHead::Modes)) {
        opHead->mPropsMap[WQt::OutputHead::Modes] = QVariant::fromValue<QList<WQt::OutputMode *>>(QList<WQt::OutputMode *>());
    }

    WQt::OutputMode *opMode = new WQt::OutputMode(mode, opHead);

    connect(opMode, &WQt::OutputMode::finished, [=]() {
        opHead->mModes.removeAll(opMode);
    });
    opHead->mModes << opMode;

    Q_EMIT opHead->changed(WQt::OutputHead::Modes);
}

void WQt::OutputHead::handleEnabled(void *data, zwlr_output_head_v1 *, int32_t yes)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Enabled] = (bool)yes;
    Q_EMIT opHead->changed(WQt::OutputHead::Enabled);
}

void WQt::OutputHead::handleCurrentMode(void *data, zwlr_output_head_v1 *, zwlr_output_mode_v1 *curMode)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    for (auto *mode : opHead->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
        if (mode->get() == curMode)
            opHead->mCurrentMode = mode;
    }

    Q_EMIT opHead->changed(WQt::OutputHead::CurrentMode);
}

void WQt::OutputHead::handlePosition(void *data, zwlr_output_head_v1 *, int32_t x, int32_t y)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Position] = QPoint(x, y);
    Q_EMIT opHead->changed(WQt::OutputHead::Position);
}

void WQt::OutputHead::handleTransform(void *data, zwlr_output_head_v1 *, int32_t transform)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Transform] = transform;
    Q_EMIT opHead->changed(WQt::OutputHead::Transform);
}

void WQt::OutputHead::handleScale(void *data, zwlr_output_head_v1 *, wl_fixed_t scale)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Scale] = wl_fixed_to_double(scale);
    Q_EMIT opHead->changed(WQt::OutputHead::Scale);
}

void WQt::OutputHead::handleFinished(void *data, zwlr_output_head_v1 *)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    Q_EMIT opHead->finished();
}

void WQt::OutputHead::handleMake(void *data, zwlr_output_head_v1 *, const char *make)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Make] = QString::fromUtf8(make);
    Q_EMIT opHead->changed(WQt::OutputHead::Make);
}

void WQt::OutputHead::handleModel(void *data, zwlr_output_head_v1 *, const char *model)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::Model] = QString::fromUtf8(model);
    Q_EMIT opHead->changed(WQt::OutputHead::Model);
}

void WQt::OutputHead::handleSerialNumber(void *data, zwlr_output_head_v1 *, const char *serialNo)
{
    auto *opHead = reinterpret_cast<WQt::OutputHead *>(data);
    opHead->mPropsMap[WQt::OutputHead::SerialNumber] = QString::fromUtf8(serialNo);
    Q_EMIT opHead->changed(WQt::OutputHead::SerialNumber);
}

void WQt::OutputHead::handleAdaptiveSync(void *data, zwlr_output_head_v1 *, uint32_t state)
{
    Q_UNUSED(data)
    Q_UNUSED(state)
}

/**
 * Output Mode
 * This describes a mode of an output head.
 * Obtained from OutputHead::property( Modes )
 * or from OutputHead::property( CurrentMode )
 */

WQt::OutputMode::OutputMode(QObject *parent)
    : QObject(parent)
{
}

WQt::OutputMode::OutputMode(zwlr_output_mode_v1 *mode, QObject *parent)
    : QObject(parent)
    , mObj(mode)
{
    setupListeners();
}

WQt::OutputMode::OutputMode(const WQt::OutputMode &otherMode)
    : QObject()
{
    mObj = otherMode.mObj;

    mSize = otherMode.mSize;
    mRefreshRate = otherMode.mRefreshRate;
    mIsPreferred = otherMode.mIsPreferred;
}

WQt::OutputMode::~OutputMode()
{
    if (mObj)
        zwlr_output_mode_v1_destroy(mObj);
}

QSize WQt::OutputMode::size()
{
    return mSize;
}

int32_t WQt::OutputMode::refreshRate()
{
    return mRefreshRate;
}

bool WQt::OutputMode::isPreferred()
{
    return mIsPreferred;
}

zwlr_output_mode_v1 *WQt::OutputMode::get()
{
    return mObj;
}

void WQt::OutputMode::setupListeners()
{
    static const zwlr_output_mode_v1_listener listener = {
        &WQt::OutputMode::handleSize,
        &WQt::OutputMode::handleRefreshRate,
        &WQt::OutputMode::handlePreferred,
        &WQt::OutputMode::handleFinished,
    };
    zwlr_output_mode_v1_add_listener(mObj, &listener, this);
}

void WQt::OutputMode::handleSize(void *data, zwlr_output_mode_v1 *, int32_t width, int32_t height)
{
    auto *opMode = reinterpret_cast<WQt::OutputMode *>(data);
    opMode->mSize = QSize(width, height);
    Q_EMIT opMode->sizeChanged(opMode->mSize);
}

void WQt::OutputMode::handleRefreshRate(void *data, zwlr_output_mode_v1 *, int32_t refreshRate)
{
    auto *opMode = reinterpret_cast<WQt::OutputMode *>(data);
    opMode->mRefreshRate = refreshRate;
    Q_EMIT opMode->refreshRateChanged(refreshRate);
}

void WQt::OutputMode::handlePreferred(void *data, zwlr_output_mode_v1 *)
{
    auto *opMode = reinterpret_cast<WQt::OutputMode *>(data);
    opMode->mIsPreferred = true;
    Q_EMIT opMode->setAsPreferred();
}

void WQt::OutputMode::handleFinished(void *data, zwlr_output_mode_v1 *)
{
    auto *opMode = reinterpret_cast<WQt::OutputMode *>(data);
    Q_EMIT opMode->finished();
}

/**
 * Output Configuration
 * We can configure all the outputs using this object
 * Obtained from OutputManager::createConfiguration()
 */

WQt::OutputConfiguration::OutputConfiguration(struct ::zwlr_output_configuration_v1 *config, QObject *parent)
    : QObject(parent)
    , QtWayland::zwlr_output_configuration_v1(config)
{
}

WQt::OutputConfiguration::~OutputConfiguration()
{
    QtWayland::zwlr_output_configuration_v1::destroy();
}

WQt::OutputConfigurationHead *WQt::OutputConfiguration::enableHead(WQt::OutputHead *head)
{
    qCDebug(DccWayQt) << "OutputConfiguration::enableHead";
    return new WQt::OutputConfigurationHead(QtWayland::zwlr_output_configuration_v1::enable_head(head->get()), this);
}

void WQt::OutputConfiguration::disableHead(WQt::OutputHead *head)
{
    qCDebug(DccWayQt) << "OutputConfiguration::disableHead";
    QtWayland::zwlr_output_configuration_v1::disable_head(head->get());
}

void WQt::OutputConfiguration::apply()
{
    qCDebug(DccWayQt) << "OutputConfiguration::apply";
    QtWayland::zwlr_output_configuration_v1::apply();
}

void WQt::OutputConfiguration::test()
{
    qCDebug(DccWayQt) << "OutputConfiguration::test";
    QtWayland::zwlr_output_configuration_v1::test();
}

void WQt::OutputConfiguration::zwlr_output_configuration_v1_succeeded()
{
    qCDebug(DccWayQt) << "OutputConfiguration::succeeded";
    Q_EMIT succeeded();
}

void WQt::OutputConfiguration::zwlr_output_configuration_v1_failed()
{
    qCWarning(DccWayQt) << "OutputConfiguration::failed";
    Q_EMIT failed();
}

void WQt::OutputConfiguration::zwlr_output_configuration_v1_cancelled()
{
    qCDebug(DccWayQt) << "OutputConfiguration::cancelled";
    Q_EMIT canceled();
}

/**
 * Output Configuration Head
 * We can set the resolution and refresh rate for a given head
 * Obtained from OutputConfiguration::enableHead()
 */

WQt::OutputConfigurationHead::OutputConfigurationHead(zwlr_output_configuration_head_v1 *configHead, QObject *parent)
    : QObject(parent)
    , mObj(configHead)
{
}

WQt::OutputConfigurationHead::~OutputConfigurationHead()
{
    if (mObj)
        zwlr_output_configuration_head_v1_destroy(mObj);
}

void WQt::OutputConfigurationHead::setMode(WQt::OutputMode *mode)
{
    qCDebug(DccWayQt) << "OutputConfigurationHead::setMode" << mode->size() << mode->refreshRate();
    zwlr_output_configuration_head_v1_set_mode(mObj, mode->get());
}

void WQt::OutputConfigurationHead::setCustomMode(QSize size, int32_t refresh)
{
    qCDebug(DccWayQt) << "OutputConfigurationHead::setCustomMode" << size << refresh;
    zwlr_output_configuration_head_v1_set_custom_mode(mObj, size.width(), size.height(), refresh);
}

void WQt::OutputConfigurationHead::setPosition(QPoint pos)
{
    qCDebug(DccWayQt) << "OutputConfigurationHead::setPosition" << pos;
    zwlr_output_configuration_head_v1_set_position(mObj, pos.x(), pos.y());
}

void WQt::OutputConfigurationHead::setTransform(int32_t transform)
{
    qCDebug(DccWayQt) << "OutputConfigurationHead::setTransform" << transform;
    zwlr_output_configuration_head_v1_set_transform(mObj, transform);
}

void WQt::OutputConfigurationHead::setScale(qreal scale)
{
    qCDebug(DccWayQt) << "OutputConfigurationHead::setScale" << scale;
    zwlr_output_configuration_head_v1_set_scale(mObj, wl_fixed_from_double(scale));
}
