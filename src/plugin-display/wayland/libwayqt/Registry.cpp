/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Marcus Britanicus (https://gitlab.com/marcusbritanicus)
 * Copyright (c) 2021 Abrar (https://gitlab.com/s96Abrar)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include "treeland-output-management-client-protocol.h"
#include "wayland-client-protocol.h"
#include "wlr-output-management-unstable-v1-client-protocol.h"

#include <Output.hpp>
#include <OutputManager.hpp>
#include <Registry.hpp>
#include <TreeLandOutputManager.hpp>

#include <QCoreApplication>
#include <QDebug>
#include <QThread>

/* Convenience functions */
void WQt::Registry::globalAnnounce(
        void *data, struct wl_registry *, uint32_t name, const char *interface, uint32_t version)
{
    auto r = reinterpret_cast<WQt::Registry *>(data);

    r->handleAnnounce(name, interface, version);
}

void WQt::Registry::globalRemove(void *data, struct wl_registry *, uint32_t name)
{
    // who cares :D
    // but we will call WQt::Registry::handleRemove just for the heck of it

    auto r = reinterpret_cast<WQt::Registry *>(data);

    r->handleRemove(name);
}

const struct wl_registry_listener WQt::Registry::mRegListener = {
    globalAnnounce,
    globalRemove,
};

WQt::Registry::Registry(wl_display *wlDisplay)
{
    mWlDisplay = wlDisplay;
    mObj = wl_display_get_registry(mWlDisplay);

    if (wl_proxy_get_listener((wl_proxy *)mObj) != &mRegListener) {
        wl_registry_add_listener(mObj, &mRegListener, this);
    }

    wl_display_roundtrip(mWlDisplay);
}

WQt::Registry::~Registry()
{
    wl_registry_destroy(mObj);

    wl_seat_destroy(mWlSeat);
    wl_shm_destroy(mWlShm);

    for (WQt::Output *op : mOutputs) {
        delete op;
    }

    if (mOutputMgr != nullptr) {
        delete mOutputMgr;
    }

    if (mTreeLandOutputMgr != nullptr) {
        delete mTreeLandOutputMgr;
    }
}

void WQt::Registry::setup()
{
    if (mIsSetup == false) {
        mIsSetup = true;

        for (WQt::Registry::ErrorType et : pendingErrors) {
            emit errorOccured(et);
        }

        for (WQt::Registry::Interface iface : pendingInterfaces) {
            emit interfaceRegistered(iface);
        }

        for (WQt::Output *op : pendingOutputs) {
            emit outputAdded(op);
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

bool WQt::Registry::waitForInterface(WQt::Registry::Interface id, int timeout)
{
    int t = 0;

    while (t < timeout) {
        if (mRegisteredInterfaces.contains(id)) {
            return true;
        }

        /** We will take 10 ms nap */
        QThread::msleep(10);

        /** Increment the timer */
        t += 10;

        /** Flush the queue */
        QCoreApplication::processEvents();
    }

    return false;
}

WQt::OutputManager *WQt::Registry::outputManager()
{
    return mOutputMgr;
}

WQt::TreeLandOutputManager *WQt::Registry::treeLandOutputManager()
{
    return mTreeLandOutputMgr;
}

void WQt::Registry::handleAnnounce(uint32_t name, const char *interface, uint32_t version)
{
    /**
     * We really don't care about wl_seat version right now.
     */
    if (strcmp(interface, wl_seat_interface.name) == 0) {
        mWlSeat = (wl_seat *)wl_registry_bind(mObj, name, &wl_seat_interface, version);

        if (!mWlSeat) {
            emitError(WQt::Registry::EmptySeat);
        }
    }

    /**
     * We really don't care about wl_shm version right now.
     */
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

    /**
     * We really don't care about wl_output version right now.
     */
    if (strcmp(interface, wl_output_interface.name) == 0) {
        wl_output *op = (wl_output *)wl_registry_bind(mObj, name, &wl_output_interface, version);

        if (op) {
            mOutputs[name] = new WQt::Output(op);
            emitOutput(mOutputs[name], true);
        }
    }

    /**
     * We've implemented version 2.
     * And wlroots 0.15.0 has version 2 available.
     */
    else if (strcmp(interface, zwlr_output_manager_v1_interface.name) == 0) {
        mWlrOutputMgr = (zwlr_output_manager_v1 *)
                wl_registry_bind(mObj, name, &zwlr_output_manager_v1_interface, 2);

        if (!mWlrOutputMgr) {
            emitError(WQt::Registry::EmptyOutputManager);
        }

        else {
            mOutputMgr = new WQt::OutputManager(mWlrOutputMgr);

            mRegisteredInterfaces << OutputManagerInterface;
            emitInterface(OutputManagerInterface, true);
        }
    }

    else if (strcmp(interface, treeland_output_manager_v1_interface.name) == 0) {
        m_treeland_output_mgr = (treeland_output_manager_v1 *)
                wl_registry_bind(mObj, name, &treeland_output_manager_v1_interface, 1);
        if (!m_treeland_output_mgr) {
            emitError(WQt::Registry::EmptyTreeLandOuputManager);
        } else {
            mTreeLandOutputMgr = new WQt::TreeLandOutputManager(m_treeland_output_mgr);

            mRegisteredInterfaces << TreeLandOutputManagerInterface;
            emitInterface(TreeLandOutputManagerInterface, true);
        }
    }
}

void WQt::Registry::handleRemove(uint32_t name)
{
    /**
     * While we do not care about most of the handleRemove,
     * we need to worry about the wl_output * objects.
     */
    if (mOutputs.keys().contains(name)) {
        WQt::Output *output = mOutputs.take(name);
        emitOutput(output, false);
    }
}

void WQt::Registry::emitError(ErrorType et)
{
    if (mIsSetup) {
        emit errorOccured(et);
    }

    else {
        pendingErrors << et;
    }
}

void WQt::Registry::emitOutput(WQt::Output *op, bool added)
{
    if (mIsSetup) {
        if (added) {
            emit outputAdded(op);
        }

        else {
            emit outputRemoved(op);
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
            emit interfaceRegistered(iface);
        }

        else {
            emit interfaceDeregistered(iface);
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
