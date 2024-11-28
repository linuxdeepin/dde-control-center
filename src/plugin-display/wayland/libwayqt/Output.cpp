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

#include "wayland-client-protocol.h"

#include "Output.hpp"
#include <wayland-client.h>

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QObject>
#include <QThread>

WQt::Output::Output(wl_output *op)
{
    mObj = op;

    wl_output_add_listener(mObj, &mListener, this);
}

WQt::Output::~Output()
{
    wl_output_destroy(mObj);
}

QString WQt::Output::name()
{
    return mName;
}

QString WQt::Output::description()
{
    return mDescr;
}

QString WQt::Output::make()
{
    return mMake;
}

QString WQt::Output::model()
{
    return mModel;
}

QPoint WQt::Output::position()
{
    return mPos;
}

QSize WQt::Output::physicalSize()
{
    return mPhysicalSize;
}

WQt::Output::OutputMode WQt::Output::mode()
{
    return mMode;
}

WQt::Output::SubpixelGeometry WQt::Output::subpixelGeometry()
{
    return (WQt::Output::SubpixelGeometry)mSubPixel;
}

WQt::Output::Rotation WQt::Output::transform()
{
    return (WQt::Output::Rotation)mTransform;
}

int32_t WQt::Output::scale()
{
    return mScale;
}

void WQt::Output::waitForReady()
{
    if (mDone) {
        return;
    }

    do {
        QThread::usleep(100);
        qApp->processEvents();
    } while (not mDone);
}

wl_output *WQt::Output::get()
{
    return mObj;
}

void WQt::Output::handleGeometryEvent(void *data,
                                      struct wl_output *,
                                      int32_t x,
                                      int32_t y,
                                      int32_t w,
                                      int32_t h,
                                      int32_t e,
                                      const char *f,
                                      const char *g,
                                      int32_t t)
{
    Output *output = reinterpret_cast<WQt::Output *>(data);

    output->mPos = QPoint(x, y);
    output->mPhysicalSize = QSize(w, h);
    output->mSubPixel = e;
    output->mMake = QString(f);
    output->mModel = QString(g);
    output->mTransform = t;
}

void WQt::Output::handleModeEvent(void *data,
                                  struct wl_output *,
                                  uint32_t current,
                                  int32_t xres,
                                  int32_t yres,
                                  int32_t refresh)
{
    Output *output = reinterpret_cast<WQt::Output *>(data);

    if (current) {
        output->mMode = { QSize(xres, yres), refresh, true };
    }
}

void WQt::Output::handleDone(void *data, struct wl_output *)
{
    Output *output = reinterpret_cast<WQt::Output *>(data);

    output->mDone = true;
}

void WQt::Output::handleScale(void *data, struct wl_output *, int32_t scale)
{
    Output *output = reinterpret_cast<WQt::Output *>(data);

    output->mScale = scale;
}

void WQt::Output::handleNameEvent(void *data, struct wl_output *, const char *name)
{
    Output *output = reinterpret_cast<WQt::Output *>(data);

    output->mName = QString(name);
}

void WQt::Output::handleDescriptionEvent(void *data, struct wl_output *, const char *descr)
{
    Output *output = reinterpret_cast<WQt::Output *>(data);

    output->mDescr = descr;
}

const wl_output_listener WQt::Output::mListener = {
    handleGeometryEvent, handleModeEvent, handleDone,
    handleScale,         handleNameEvent, handleDescriptionEvent,
};
