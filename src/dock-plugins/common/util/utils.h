// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UTILS
#define UTILS

#include <QPixmap>
#include <QImageReader>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QJsonDocument>

#include <DWindowManagerHelper>

namespace Utils {

#define ICBC_CONF_FILE "/etc/deepin/icbc.conf"

const bool IS_WAYLAND_DISPLAY = !qgetenv("WAYLAND_DISPLAY").isEmpty();

/* convert 'some-key' to 'someKey' or 'SomeKey'.
 * the second form is needed for appending to 'set' for 'setSomeKey'
 */
inline QString qtify_name(const char *name)
{
    bool next_cap = false;
    QString result;

    while (*name) {
        if (*name == '-') {
            next_cap = true;
        } else if (next_cap) {
            result.append(QChar(*name).toUpper().toLatin1());
            next_cap = false;
        } else {
            result.append(*name);
        }

        name++;
    }

    return result;
}

inline QPixmap renderSVG(const QString &path, const QSize &size, const qreal devicePixelRatio)
{
    QImageReader reader;
    QPixmap pixmap;
    reader.setFileName(path);
    if (reader.canRead()) {
        reader.setScaledSize(size * devicePixelRatio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(devicePixelRatio);
    }
    else {
        pixmap.load(path);
    }

    return pixmap;
}

inline QScreen *screenAt(const QPoint &point) {
    for (QScreen *screen : qApp->screens()) {
        const QRect r { screen->geometry() };
        const QRect rect { r.topLeft(), r.size() * screen->devicePixelRatio() };
        if (rect.contains(point)) {
            return screen;
        }
    }

    return nullptr;
}

//!!! 注意:这里传入的QPoint是未计算缩放的
inline QScreen *screenAtByScaled(const QPoint &point) {
    for (QScreen *screen : qApp->screens()) {
        const QRect r { screen->geometry() };
        QRect rect { r.topLeft(), r.size() * screen->devicePixelRatio() };
        if (rect.contains(point)) {
            return screen;
        }
    }

    return nullptr;
}

/**
 * @brief json对象转为json字符串
 *
 * @param QJsonObject对象
 */
inline QString toJson(const QJsonObject &jsonObj)
{
    QJsonDocument doc;
    doc.setObject(jsonObj);
    return doc.toJson();
}

inline QJsonObject getRootObj(const QString &jsonStr)
{
    QJsonParseError jsonParseError;
    const QJsonDocument &resultDoc = QJsonDocument::fromJson(jsonStr.toLocal8Bit(), &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError || resultDoc.isEmpty()) {
        qWarning() << "Result json parse error";
        return QJsonObject();
    }

    return resultDoc.object();
}

inline bool hasBlurWindow()
{
    return DTK_GUI_NAMESPACE::DWindowManagerHelper::instance()->hasBlurWindow() || IS_WAYLAND_DISPLAY;
}

// 将控制中心的透明度映射为任务栏的透明度
inline float mapOpacityValue(float value)
{
    // 控制中心透明度区间
    const float fromMax = 1.0;
    const float fromMin = 0.0;
    // 任务栏透明度区间
    const float toMax = 1.0;
    const float toMin = 0.2;
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

}

#endif // UTILS
