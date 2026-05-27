// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wayland-client-protocol.h"

#include "Output.h"
#include "OutputManager.h"
#include "Registry.h"
#include "TreeLandOutputManager.h"
#include "VirtualOutputManager.h"
#include "WallpaperManager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QThread>

#include "WayQtLogging.h"

Q_LOGGING_CATEGORY(DccWayQt, "dcc-wayqt")

/* Convenience functions */
void WQt::Registry::globalAnnounce(
        void *data, struct wl_registry *, uint32_t name, const char *interface, uint32_t version)
{
    auto r = reinterpret_cast<WQt::Registry *>(data);
    r->handleAnnounce(name, interface, version);
}

void WQt::Registry::globalRemove(void *data, struct wl_registry *, uint32_t name)
{
    auto r = reinterpret_cast<WQt::Registry *>(data);
    r->handleRemove(name);
}

const struct wl_registry_listener WQt::Registry::mRegListener = {
    globalAnnounce,
    globalRemove,
};

WQt::Registry::Registry(wl_display *wlDisplay, QObject *parent)
    : QObject(parent)
{
    mWlDisplay = wlDisplay;
    mObj = wl_display_get_registry(mWlDisplay);

    if (wl_proxy_get_listener((wl_proxy *)mObj) != &mRegListener) {
        wl_registry_add_listener(mObj, &mRegListener, this);
    }
    qCDebug(DccWayQt) << "Registry created" << mWlDisplay << mObj;
    wl_display_roundtrip(mWlDisplay);
}

WQt::Registry::~Registry()
{
    wl_registry_destroy(mObj);
    qCDebug(DccWayQt) << "Registry destroyed";
    wl_seat_destroy(mWlSeat);
    wl_shm_destroy(mWlShm);

    for (WQt::Output *op : mOutputs) {
        delete op;
    }

    delete mOutputMgr;
    delete mTreeLandOutputMgr;
    delete mVirtualOutputMgr;
    delete mWallpaperMgr;
}

void WQt::Registry::setup()
{
    if (!mIsSetup) {
        mIsSetup = true;

        for (WQt::Registry::ErrorType et : pendingErrors) {
            Q_EMIT errorOccured(et);
        }

        for (WQt::Registry::Interface iface : pendingInterfaces) {
            Q_EMIT interfaceRegistered(iface);
        }

        for (WQt::Output *op : pendingOutputs) {
            Q_EMIT outputAdded(op);
        }
    }
}

wl_registry *WQt::Registry::get()
{
    return mObj;
}

wl_display *WQt::Registry::waylandDisplay()
{
    return mWlDisplay;
}

wl_seat *WQt::Registry::waylandSeat()
{
    return mWlSeat;
}

wl_shm *WQt::Registry::waylandShm()
{
    return mWlShm;
}

QList<WQt::Output *> WQt::Registry::waylandOutputs()
{
    return mOutputs.values();
}

QList<uint32_t> WQt::Registry::registeredInterfaces()
{
    return mRegisteredInterfaces;
}

WQt::OutputManager *WQt::Registry::outputManager()
{
    return mOutputMgr;
}

WQt::TreeLandOutputManager *WQt::Registry::treeLandOutputManager()
{
    return mTreeLandOutputMgr;
}

WQt::VirtualOutputManager *WQt::Registry::virtualOutputManager()
{
    return mVirtualOutputMgr;
}

WQt::WallpaperManager *WQt::Registry::wallpaperManager()
{
    return mWallpaperMgr;
}

void WQt::Registry::handleAnnounce(uint32_t name, const char *interface, uint32_t version)
{
    if (strcmp(interface, wl_seat_interface.name) == 0) {
        mWlSeat = (wl_seat *)wl_registry_bind(mObj, name, &wl_seat_interface, version);

        if (!mWlSeat) {
            emitError(WQt::Registry::EmptySeat);
        }
    }

    if (strcmp(interface, wl_shm_interface.name) == 0) {
        mWlShm = (wl_shm *)wl_registry_bind(mObj, name, &wl_shm_interface, version);

        if (!mWlShm) {
            emitError(WQt::Registry::EmptyShm);
        }

        else {
            mRegisteredInterfaces << ShmInterface;
            emitInterface(ShmInterface, true);
        }
    }

    if (strcmp(interface, wl_output_interface.name) == 0) {
        wl_output *op = (wl_output *)wl_registry_bind(mObj, name, &wl_output_interface, version);
        qCInfo(DccWayQt) << "wl_output added:" << op << "name:" << name << "version:" << version << "total:" << mOutputs.size();
        if (op) {
            auto outputObj = new WQt::Output(op, this);
            mOutputs[name] = outputObj;
            emitOutput(outputObj, true);
        }
    }

    else if (strcmp(interface, zwlr_output_manager_v1_interface.name) == 0) {
        qCDebug(DccWayQt) << "zwlr_output_manager_v1 announced:" << name << version << "thread:" << QThread::currentThread();
        if (!mOutputMgr) {
            mOutputMgr = new WQt::OutputManager(this);
            qCDebug(DccWayQt) << "OutputManager created, active:" << mOutputMgr->isActive() << "thread:" << QThread::currentThread();
            connect(mOutputMgr, &WQt::OutputManager::activeChanged, this, [this]() {
                if (!mOutputMgr->isActive()) {
                    emitError(WQt::Registry::EmptyOutputManager);
                } else {
                    mRegisteredInterfaces << OutputManagerInterface;
                    emitInterface(OutputManagerInterface, true);
                }
            });
        }
    }

    else if (strcmp(interface, treeland_output_manager_v1_interface.name) == 0) {
        if (!mTreeLandOutputMgr) {
            mTreeLandOutputMgr = new WQt::TreeLandOutputManager(this);

            connect(mTreeLandOutputMgr, &WQt::TreeLandOutputManager::activeChanged, this, [this]() {
                if (!mTreeLandOutputMgr->isActive()) {
                    emitError(WQt::Registry::EmptyTreeLandOuputManager);
                } else {
                    mRegisteredInterfaces << TreeLandOutputManagerInterface;
                    emitInterface(TreeLandOutputManagerInterface, true);
                }
            });
        }
    }

    else if (strcmp(interface, treeland_virtual_output_manager_v1_interface.name) == 0) {
        if (!mVirtualOutputMgr) {
            mVirtualOutputMgr = new WQt::VirtualOutputManager(this);

            connect(mVirtualOutputMgr, &WQt::VirtualOutputManager::activeChanged, this, [this]() {
                if (!mVirtualOutputMgr->isActive()) {
                    emitError(WQt::Registry::EmptyVirtualOutputManager);
                } else {
                    mRegisteredInterfaces << VirtualOutputManagerInterface;
                    emitInterface(VirtualOutputManagerInterface, true);
                }
            });
        }
    }

    else if (strcmp(interface, treeland_wallpaper_manager_v1_interface.name) == 0) {
        if (!mWallpaperMgr) {
            mWallpaperMgr = new WQt::WallpaperManager(this);

            connect(mWallpaperMgr, &WQt::WallpaperManager::activeChanged, this, [this]() {
                if (!mWallpaperMgr->isActive()) {
                    emitError(WQt::Registry::EmptyWallpaperManager);
                } else {
                    mRegisteredInterfaces << WallpaperManagerInterface;
                    emitInterface(WallpaperManagerInterface, true);
                }
            });
        }
    }
}

void WQt::Registry::handleRemove(uint32_t name)
{
    if (mOutputs.keys().contains(name)) {
        WQt::Output *output = mOutputs.take(name);
        emitOutput(output, false);
    }
}

void WQt::Registry::emitError(ErrorType et)
{
    qCWarning(DccWayQt) << "Registry error:" << et;
    if (mIsSetup) {
        Q_EMIT errorOccured(et);
    }

    else {
        pendingErrors << et;
    }
}

void WQt::Registry::emitOutput(WQt::Output *op, bool added)
{
    if (mIsSetup) {
        if (added) {
            Q_EMIT outputAdded(op);
        }

        else {
            Q_EMIT outputRemoved(op);
        }
    }

    else {
        if (added) {
            pendingOutputs << op;
        }

        else {
            pendingOutputs.removeAll(op);
        }
    }
}

void WQt::Registry::emitInterface(WQt::Registry::Interface iface, bool added)
{
    if (mIsSetup) {
        if (added) {
            Q_EMIT interfaceRegistered(iface);
        }

        else {
            Q_EMIT interfaceDeregistered(iface);
        }
    }

    else {
        if (added) {
            pendingInterfaces << iface;
        }

        else {
            pendingInterfaces.removeAll(iface);
        }
    }
}
