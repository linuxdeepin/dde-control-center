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

#include "GammaControl.hpp"

#include "wlr-gamma-control-unstable-v1-client-protocol.h"

#include <wayland-client.h>

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QObject>
#include <QThread>

WQt::GammaControlManager::GammaControlManager(zwlr_gamma_control_manager_v1 *opMgr)
{
    mObj = opMgr;
}

WQt::GammaControlManager::~GammaControlManager()
{
    zwlr_gamma_control_manager_v1_destroy(mObj);
}

WQt::GammaControl *WQt::GammaControlManager::getGammaControl(wl_output *output)
{
    zwlr_gamma_control_v1 *wlrGammaCtrl =
            zwlr_gamma_control_manager_v1_get_gamma_control(mObj, output);
    GammaControl *gammaCtrl = new GammaControl(wlrGammaCtrl);

    return gammaCtrl;
}

WQt::GammaControl::GammaControl(zwlr_gamma_control_v1 *opMgr)
{
    mObj = opMgr;
    zwlr_gamma_control_v1_add_listener(mObj, &mListener, this);
}

WQt::GammaControl::~GammaControl()
{
    zwlr_gamma_control_v1_destroy(mObj);
}

uint32_t WQt::GammaControl::gammaSize()
{
    while (not mGammaSize) {
        QThread::usleep(100);

        qApp->processEvents();
    }

    return mGammaSize;
}

void WQt::GammaControl::setGamma(int32_t fd)
{
    zwlr_gamma_control_v1_set_gamma(mObj, fd);
}

void WQt::GammaControl::handleGammaSize(void *data, struct zwlr_gamma_control_v1 *, uint32_t gSize)
{
    WQt::GammaControl *gammaCtrl = reinterpret_cast<WQt::GammaControl *>(data);

    gammaCtrl->mGammaSize = gSize;
    emit gammaCtrl->gammaSizeChanged(gSize);
}

void WQt::GammaControl::handleFailed(void *data, struct zwlr_gamma_control_v1 *)
{
    WQt::GammaControl *pwr = reinterpret_cast<WQt::GammaControl *>(data);
    emit pwr->failed();
}

const zwlr_gamma_control_v1_listener WQt::GammaControl::mListener = { handleGammaSize,
                                                                      handleFailed };
