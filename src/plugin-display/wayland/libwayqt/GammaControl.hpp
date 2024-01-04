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

#pragma once

#include <wayland-client-protocol.h>

#include <QMap>
#include <QObject>
#include <QRect>
#include <QString>

struct wl_buffer;
struct wl_output;
struct zwlr_gamma_control_manager_v1;
struct zwlr_gamma_control_v1;
struct zwlr_gamma_control_v1_listener;

namespace WQt {
class GammaControlManager;
class GammaControl;
} // namespace WQt

class WQt::GammaControlManager : public QObject
{
    Q_OBJECT;

public:
    GammaControlManager(zwlr_gamma_control_manager_v1 *);
    ~GammaControlManager();

    GammaControl *getGammaControl(wl_output *);

    zwlr_gamma_control_manager_v1 *get();

private:
    zwlr_gamma_control_manager_v1 *mObj;
};

class WQt::GammaControl : public QObject
{
    Q_OBJECT;

public:
    enum Error {
        InvalidGamma = 1,
    };

    GammaControl(zwlr_gamma_control_v1 *);
    ~GammaControl();

    /** Set the output mode */
    uint32_t gammaSize();

    /** Set the output mode */
    void setGamma(int32_t);

    zwlr_gamma_control_v1 *get();

private:
    static void handleGammaSize(void *, struct zwlr_gamma_control_v1 *, uint32_t);
    static void handleFailed(void *, struct zwlr_gamma_control_v1 *);

    static const zwlr_gamma_control_v1_listener mListener;

    zwlr_gamma_control_v1 *mObj;

    uint32_t mGammaSize = 0;

Q_SIGNALS:
    void failed();
    void gammaSizeChanged(uint32_t);
};
