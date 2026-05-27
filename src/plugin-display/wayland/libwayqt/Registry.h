// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QHash>
#include <QObject>

struct wl_registry;
struct wl_display;
struct wl_seat;
struct wl_shm;
struct wl_output;

struct wl_registry_listener;

namespace WQt {
class Registry;
class XdgShell;

class Output;
class OutputManager;
class TreeLandOutputManager;
class VirtualOutputManager;
class WallpaperManager;
} // namespace WQt

class WQt::Registry : public QObject
{
    Q_OBJECT;

public:
    enum ErrorType {
        EmptyShm, //
        EmptyIdle,
        EmptySeat,
        EmptyXdgWmBase,
        EmptyCompositor,
        EmptyOutputManager,
        EmptyTreeLandOuputManager,
        EmptyVirtualOutputManager,
        EmptyWallpaperManager
    };

    Q_ENUM(ErrorType);

    enum Interface {
        ShmInterface, //
        IdleInterface,
        SeatInterface,
        WlrIdleInterface,
        XdgWmBaseInterface,
        CompositorInterface,
        OutputManagerInterface,
        TreeLandOutputManagerInterface,
        VirtualOutputManagerInterface,
        WallpaperManagerInterface
    };

    Q_ENUM(Interface);

    Registry(wl_display *wlDisplay, QObject *parent = nullptr);
    ~Registry();

    void setup();

    wl_registry *get();

    wl_display *waylandDisplay();
    wl_seat *waylandSeat();
    wl_shm *waylandShm();

    QList<WQt::Output *> waylandOutputs();

    /** List the already registered interfaces */
    QList<uint32_t> registeredInterfaces();

    /**
     * OutputManager - auto-bound via QWaylandClientExtensionTemplate
     */
    WQt::OutputManager *outputManager();

    /**
     * TreeLandOutputManager - auto-bound via QWaylandClientExtensionTemplate
     */
    WQt::TreeLandOutputManager *treeLandOutputManager();

    /**
     * VirtualOutputManager - auto-bound via QWaylandClientExtensionTemplate
     */
    WQt::VirtualOutputManager *virtualOutputManager();

    WQt::WallpaperManager *wallpaperManager();

private:
    wl_registry *mObj = nullptr;
    wl_display *mWlDisplay = nullptr;
    wl_seat *mWlSeat = nullptr;
    wl_shm *mWlShm = nullptr;

    QHash<uint32_t, WQt::Output *> mOutputs;
    QList<uint32_t> mRegisteredInterfaces;

    WQt::OutputManager *mOutputMgr = nullptr;
    WQt::TreeLandOutputManager *mTreeLandOutputMgr = nullptr;
    WQt::VirtualOutputManager *mVirtualOutputMgr = nullptr;
    WQt::WallpaperManager *mWallpaperMgr = nullptr;

    static const wl_registry_listener mRegListener;
    static void globalAnnounce(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    static void globalRemove(void *data, wl_registry *registry, uint32_t name);

    void handleAnnounce(uint32_t name, const char *interface, uint32_t version);
    void handleRemove(uint32_t name);

    QList<WQt::Registry::ErrorType> pendingErrors;
    QList<WQt::Output *> pendingOutputs;
    QList<WQt::Registry::Interface> pendingInterfaces;

    bool mIsSetup = false;

    void emitError(ErrorType);
    void emitOutput(WQt::Output *, bool);
    void emitInterface(WQt::Registry::Interface, bool);

Q_SIGNALS:
    void errorOccured(ErrorType et);

    void outputAdded(WQt::Output *);
    void outputRemoved(WQt::Output *);

    void interfaceRegistered(WQt::Registry::Interface);
    void interfaceDeregistered(WQt::Registry::Interface);
};
