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

struct wl_output;
struct wl_output_listener;

namespace WQt {
class Output;
}

class WQt::Output : public QObject
{
    Q_OBJECT;

public:
    enum SubpixelGeometry {
        Unknown = 0x634429,
        None,
        HorizontalRGB,
        HorizontalBGR,
        VerticalRGB,
        VerticalBGR,
    };

    enum Rotation {
        Normal = 0x951893,
        Rotate90,
        Rotate180,
        Rotate270,
        Flipped,
        Flipped90,
        Flipped180,
        Flipped270,
    };

    typedef struct output_mode_t
    {
        QSize resolution;
        int32_t refreshRate;
        bool current = false;
    } OutputMode;

    Output(wl_output *);
    ~Output();

    QString name();
    QString description();
    QString make();
    QString model();

    QPoint position();
    QSize physicalSize();
    WQt::Output::OutputMode mode();
    SubpixelGeometry subpixelGeometry();
    Rotation transform();

    int32_t scale();

    void waitForReady();

    wl_output *get();

private:
    static void handleGeometryEvent(void *,
                                    struct wl_output *,
                                    int32_t,
                                    int32_t,
                                    int32_t,
                                    int32_t,
                                    int32_t,
                                    const char *,
                                    const char *,
                                    int32_t);
    static void handleModeEvent(void *, struct wl_output *, uint32_t, int32_t, int32_t, int32_t);
    static void handleDone(void *, struct wl_output *);
    static void handleScale(void *, struct wl_output *, int32_t);
    static void handleNameEvent(void *, struct wl_output *, const char *);
    static void handleDescriptionEvent(void *, struct wl_output *, const char *);

    wl_output *mObj;
    static const wl_output_listener mListener;

    QPoint mPos;
    QSize mPhysicalSize;
    int32_t mSubPixel;
    QString mMake;
    QString mModel;
    int32_t mTransform;

    WQt::Output::OutputMode mMode;
    int32_t mScale;

    QString mName;
    QString mDescr;

    bool mDone = false;
};
