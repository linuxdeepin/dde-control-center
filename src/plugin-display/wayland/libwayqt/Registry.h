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
// struct wl_compositor;

struct wl_registry_listener;

struct xdg_wm_base;
struct zwlr_output_manager_v1;
struct zwlr_gamma_control_manager_v1;
struct treeland_output_manager_v1;

namespace WQt {
class Registry;
class XdgShell;

class Output;
class OutputManager;
class TreeLandOutputManager;
} // namespace WQt

class WQt::Registry : public QObject
{
    Q_OBJECT;

public:
    enum ErrorType {
        EmptyShm,
        EmptyIdle,
        EmptySeat,
        EmptyXdgWmBase,
        EmptyCompositor,
        EmptyOutputManager,
        EmptyTreeLandOuputManager
    };

    Q_ENUM(ErrorType);

    enum Interface {
        ShmInterface,
        IdleInterface,
        SeatInterface,
        WlrIdleInterface,
        XdgWmBaseInterface,
        CompositorInterface,
        OutputManagerInterface,
        TreeLandOutputManagerInterface
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

    /* Ready to use Wayland Classes */

    /**
     * XdgShell - Xdg Shell protocol implementation
     */
    WQt::XdgShell *xdgShell();

    /**
     * OutputManager - Output Management protocol implementation
     */
    WQt::OutputManager *outputManager();

    /**
     * TreeLandOutputManager - Primary Output Manager
     */
    WQt::TreeLandOutputManager *treeLandOutputManager();

private:
    /** Raw C pointer to this class */
    wl_registry *mObj = nullptr;

    /** wl_display object */
    wl_display *mWlDisplay = nullptr;

    /** wl_seat object */
    wl_seat *mWlSeat = nullptr;

    /** wl_shm object */
    wl_shm *mWlShm = nullptr;

    /** Connected outputs */
    QHash<uint32_t, WQt::Output *> mOutputs;

    /** List of registered interfaces */
    QList<uint32_t> mRegisteredInterfaces;

    /**
     * Output Manager Objects
     */
    zwlr_output_manager_v1 *mWlrOutputMgr = nullptr;
    WQt::OutputManager *mOutputMgr = nullptr;

    /**
     * Gamma Control Objects
     */
    zwlr_gamma_control_manager_v1 *mWlrGammaCtrl = nullptr;

    treeland_output_manager_v1 *m_treeland_output_mgr = nullptr;
    WQt::TreeLandOutputManager *mTreeLandOutputMgr = nullptr;

    static const wl_registry_listener mRegListener;
    static void globalAnnounce(void *data,
                               wl_registry *registry,
                               uint32_t name,
                               const char *interface,
                               uint32_t version);
    static void globalRemove(void *data, wl_registry *registry, uint32_t name);

    void handleAnnounce(uint32_t name, const char *interface, uint32_t version);
    void handleRemove(uint32_t name);

    QList<WQt::Registry::ErrorType> pendingErrors;
    QList<WQt::Output *> pendingOutputs;
    QList<WQt::Registry::Interface> pendingInterfaces;

    /** Flag to ensure setup() is called only once. */
    bool mIsSetup = false;

    /** emit errorOccured or store it in pending */
    void emitError(ErrorType);

    /**
     * emit output added/removed or store in pending.
     * bool indicates the state: true => added, false => removed.
     */
    void emitOutput(WQt::Output *, bool);

    /**
     * emit iInterface registered/deregistered, or store in pending.
     * bool indicates the state: true => registered, false => deregistered.
     */
    void emitInterface(WQt::Registry::Interface, bool);

signals:
    void errorOccured(ErrorType et);

    void outputAdded(WQt::Output *);
    void outputRemoved(WQt::Output *);

    void interfaceRegistered(WQt::Registry::Interface);
    void interfaceDeregistered(WQt::Registry::Interface);
};
