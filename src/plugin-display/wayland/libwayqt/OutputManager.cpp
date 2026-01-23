// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OutputManager.h"

#include "wlr-output-management-unstable-v1-client-protocol.h"

#include <wayland-client.h>

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QObject>
#include <QThread>
#include <QVariant>

/**
 * Output Manager
 * This is the beginning of all our operations.
 * Obtain a pointer to this object from Wayland Registry
 */

WQt::OutputManager::OutputManager(zwlr_output_manager_v1 *opMgr)
{
    mObj = opMgr;
    zwlr_output_manager_v1_add_listener(mObj, &mListener, this);
}

WQt::OutputManager::~OutputManager()
{
    zwlr_output_manager_v1_destroy(mObj);
}

QList<WQt::OutputHead *> WQt::OutputManager::heads()
{
    return mHeads;
}

WQt::OutputConfiguration *WQt::OutputManager::createConfiguration()
{
    return new WQt::OutputConfiguration(zwlr_output_manager_v1_create_configuration(mObj, mSerial));
}

void WQt::OutputManager::stop()
{
    zwlr_output_manager_v1_stop(mObj);
}

zwlr_output_manager_v1 *WQt::OutputManager::get()
{
    return mObj;
}

void WQt::OutputManager::handleHead(void *data, zwlr_output_manager_v1 *, zwlr_output_head_v1 *head)
{
    WQt::OutputManager *mgr = reinterpret_cast<WQt::OutputManager *>(data);

    WQt::OutputHead *opHead = new WQt::OutputHead(head);

    mgr->mHeads << opHead;

    connect(opHead, &WQt::OutputHead::finished, [=]() {
        mgr->mHeads.removeAll(opHead);
    });

    emit mgr->headAttached(opHead);
}

void WQt::OutputManager::handleDone(void *data, zwlr_output_manager_v1 *, uint32_t serial)
{
    WQt::OutputManager *mgr = reinterpret_cast<WQt::OutputManager *>(data);

    mgr->mSerial = serial;
    mgr->mIsDone = true;

    emit mgr->done();
}

void WQt::OutputManager::handleFinished(void *data, zwlr_output_manager_v1 *)
{
    WQt::OutputManager *mgr = reinterpret_cast<WQt::OutputManager *>(data);

    zwlr_output_manager_v1_destroy(mgr->mObj);
    mgr->mObj = nullptr;
}

const struct zwlr_output_manager_v1_listener WQt::OutputManager::mListener = {
    handleHead,
    handleDone,
    handleFinished,
};

/**
 * Output Head
 * Heads are obtained from OutputManager class, via signals,
 * or via OutputManager::heads() function;
 */

WQt::OutputHead::OutputHead() { }

WQt::OutputHead::OutputHead(zwlr_output_head_v1 *head)
{
    mObj = head;
    zwlr_output_head_v1_add_listener(mObj, &mListener, this);
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

void WQt::OutputHead::handleName(void *data, zwlr_output_head_v1 *, const char *name)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Name] = name;

    emit opHead->changed(WQt::OutputHead::Name);
}

void WQt::OutputHead::handleDescription(void *data, zwlr_output_head_v1 *, const char *descr)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Description] = descr;

    emit opHead->changed(WQt::OutputHead::Description);
}

void WQt::OutputHead::handlePhysicalSize(void *data,
                                         zwlr_output_head_v1 *,
                                         int32_t width,
                                         int32_t height)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::PhysicalSize] = QSize(width, height);

    emit opHead->changed(WQt::OutputHead::PhysicalSize);
}

void WQt::OutputHead::handleMode(void *data, zwlr_output_head_v1 *, zwlr_output_mode_v1 *mode)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    if (opHead->mPropsMap.contains(WQt::OutputHead::Modes)) {
        opHead->mPropsMap[WQt::OutputHead::Modes] =
                QVariant::fromValue<QList<WQt::OutputMode *>>(QList<WQt::OutputMode *>());
    }

    WQt::OutputMode *opMode = new WQt::OutputMode(mode);

    connect(opMode, &WQt::OutputMode::finished, [=]() {
        opHead->mModes.removeAll(opMode);
    });
    opHead->mModes << opMode;

    emit opHead->changed(WQt::OutputHead::Modes);
}

void WQt::OutputHead::handleEnabled(void *data, zwlr_output_head_v1 *, int32_t yes)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Enabled] = (bool)yes;

    emit opHead->changed(WQt::OutputHead::Enabled);
}

void WQt::OutputHead::handleCurrentMode(void *data,
                                        zwlr_output_head_v1 *,
                                        zwlr_output_mode_v1 *curMode)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    for (auto *mode : opHead->property(WQt::OutputHead::Modes).value<QList<WQt::OutputMode *>>()) {
        if (mode->get() == curMode)
            opHead->mCurrentMode = mode;
    }

    emit opHead->changed(WQt::OutputHead::CurrentMode);
}

void WQt::OutputHead::handlePosition(void *data, zwlr_output_head_v1 *, int32_t x, int32_t y)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Position] = QPoint(x, y);

    emit opHead->changed(WQt::OutputHead::Position);
}

void WQt::OutputHead::handleTransform(void *data, zwlr_output_head_v1 *, int32_t transform)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Transform] = transform;

    emit opHead->changed(WQt::OutputHead::Transform);
}

void WQt::OutputHead::handleScale(void *data, zwlr_output_head_v1 *, wl_fixed_t scale)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Scale] = wl_fixed_to_double(scale);

    emit opHead->changed(WQt::OutputHead::Scale);
}

void WQt::OutputHead::handleFinished(void *data, zwlr_output_head_v1 *)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    emit opHead->finished();
}

void WQt::OutputHead::handleMake(void *data, zwlr_output_head_v1 *, const char *make)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Make] = make;

    emit opHead->changed(WQt::OutputHead::Make);
}

void WQt::OutputHead::handleModel(void *data, zwlr_output_head_v1 *, const char *model)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::Model] = model;

    emit opHead->changed(WQt::OutputHead::Model);
}

void WQt::OutputHead::handleSerialNumber(void *data, zwlr_output_head_v1 *, const char *serialNo)
{
    WQt::OutputHead *opHead = reinterpret_cast<WQt::OutputHead *>(data);

    opHead->mPropsMap[WQt::OutputHead::SerialNumber] = serialNo;

    emit opHead->changed(WQt::OutputHead::SerialNumber);
}

const struct zwlr_output_head_v1_listener WQt::OutputHead::mListener = {
    handleName,        handleDescription, handlePhysicalSize, handleMode,  handleEnabled,
    handleCurrentMode, handlePosition,    handleTransform,    handleScale, handleFinished,
    handleMake,        handleModel,       handleSerialNumber,
};

/**
 * Output Mode
 * This describes a mode of an output head.
 * Obtained from OutputHead::property( Modes )
 * or from OutputHead::property( CurrentMode )
 */

WQt::OutputMode::OutputMode() { }

WQt::OutputMode::OutputMode(zwlr_output_mode_v1 *mode)
{
    mObj = mode;
    zwlr_output_mode_v1_add_listener(mObj, &mListener, this);
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

void WQt::OutputMode::handleSize(void *data, zwlr_output_mode_v1 *, int32_t width, int32_t height)
{
    WQt::OutputMode *opMode = reinterpret_cast<WQt::OutputMode *>(data);

    opMode->mSize = QSize(width, height);

    emit opMode->sizeChanged(opMode->mSize);
}

void WQt::OutputMode::handleRefreshRate(void *data, zwlr_output_mode_v1 *, int32_t refreshRate)
{
    WQt::OutputMode *opMode = reinterpret_cast<WQt::OutputMode *>(data);

    opMode->mRefreshRate = refreshRate;

    emit opMode->refreshRateChanged(refreshRate);
}

void WQt::OutputMode::handlePreferred(void *data, zwlr_output_mode_v1 *)
{
    WQt::OutputMode *opMode = reinterpret_cast<WQt::OutputMode *>(data);

    opMode->mIsPreferred = true;

    emit opMode->setAsPreferred();
}

void WQt::OutputMode::handleFinished(void *data, zwlr_output_mode_v1 *)
{
    WQt::OutputMode *opMode = reinterpret_cast<WQt::OutputMode *>(data);

    emit opMode->finished();
}

const struct zwlr_output_mode_v1_listener WQt::OutputMode::mListener = {
    handleSize,
    handleRefreshRate,
    handlePreferred,
    handleFinished,
};

/**
 * Output Configuration
 * We can configure all the outputs using this object
 * Obtained from OutputManager::createConfiguration()
 */

WQt::OutputConfiguration::OutputConfiguration(zwlr_output_configuration_v1 *config)
{
    mObj = config;
    zwlr_output_configuration_v1_add_listener(mObj, &mListener, this);
}

WQt::OutputConfiguration::~OutputConfiguration()
{
    zwlr_output_configuration_v1_destroy(mObj);
}

WQt::OutputConfigurationHead *WQt::OutputConfiguration::enableHead(WQt::OutputHead *head)
{
    return new WQt::OutputConfigurationHead(
            zwlr_output_configuration_v1_enable_head(mObj, head->get()));
}

void WQt::OutputConfiguration::disableHead(WQt::OutputHead *head)
{
    zwlr_output_configuration_v1_disable_head(mObj, head->get());
}

void WQt::OutputConfiguration::apply()
{
    zwlr_output_configuration_v1_apply(mObj);
}

void WQt::OutputConfiguration::test()
{
    zwlr_output_configuration_v1_test(mObj);
}

void WQt::OutputConfiguration::handleSucceeded(void *data, zwlr_output_configuration_v1 *)
{
    WQt::OutputConfiguration *config = reinterpret_cast<WQt::OutputConfiguration *>(data);

    emit config->succeeded();

    zwlr_output_configuration_v1_destroy(config->mObj);
}

void WQt::OutputConfiguration::handleFailed(void *data, zwlr_output_configuration_v1 *)
{
    WQt::OutputConfiguration *config = reinterpret_cast<WQt::OutputConfiguration *>(data);

    emit config->failed();

    zwlr_output_configuration_v1_destroy(config->mObj);
}

void WQt::OutputConfiguration::handleCanceled(void *data, zwlr_output_configuration_v1 *)
{
    WQt::OutputConfiguration *config = reinterpret_cast<WQt::OutputConfiguration *>(data);

    emit config->canceled();

    zwlr_output_configuration_v1_destroy(config->mObj); // need?
}

const struct zwlr_output_configuration_v1_listener WQt::OutputConfiguration::mListener = {
    handleSucceeded,
    handleFailed,
    handleCanceled,
};

/**
 * Output Configuration Head
 * We can set the resolution and refresh rate for a given head
 * Obtained from OutputConfiguration::enableHead()
 */

WQt::OutputConfigurationHead::OutputConfigurationHead(zwlr_output_configuration_head_v1 *configHead)
{
    mObj = configHead;
}

WQt::OutputConfigurationHead::~OutputConfigurationHead()
{
    zwlr_output_configuration_head_v1_destroy(mObj);
}

void WQt::OutputConfigurationHead::setMode(WQt::OutputMode *mode)
{
    zwlr_output_configuration_head_v1_set_mode(mObj, mode->get());
}

void WQt::OutputConfigurationHead::setCustomMode(QSize size, int32_t refresh)
{
    zwlr_output_configuration_head_v1_set_custom_mode(mObj, size.width(), size.height(), refresh);
}

void WQt::OutputConfigurationHead::setPosition(QPoint pos)
{
    zwlr_output_configuration_head_v1_set_position(mObj, pos.x(), pos.y());
}

void WQt::OutputConfigurationHead::setTransform(int32_t transform)
{
    zwlr_output_configuration_head_v1_set_transform(mObj, transform);
}

void WQt::OutputConfigurationHead::setScale(qreal scale)
{
    zwlr_output_configuration_head_v1_set_scale(mObj, wl_fixed_from_double(scale));
}
