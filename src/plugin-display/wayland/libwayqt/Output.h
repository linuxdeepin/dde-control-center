// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    bool isReady() const;

    wl_output *get();

Q_SIGNALS:
    void done();

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
