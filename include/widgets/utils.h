#ifndef UTILS_H
#define UTILS_H
#include "interface/namespace.h"

#include <DPlatformHandle>
#include <DSysInfo>

#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QLocale>
#include <QMargins>
#include <QMetaMethod>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QVariant>

DCORE_USE_NAMESPACE

const QMargins ZeroMargins(0, 0, 0, 0);

const int ComboxWidgetHeight = 48;
const int SwitchWidgetHeight = 36;
const int ComboxTitleWidth = 110;

const qint32 ActionIconSize = 30; // 大图标角标大小
const qint32 ActionListSize = 26; // list图标角标大小

template<typename T>
T valueByQSettings(const QStringList &configFiles,
                   const QString &group,
                   const QString &key,
                   const QVariant &failback)
{
    for (const QString &path : configFiles) {
        QSettings settings(path, QSettings::IniFormat);
        if (!group.isEmpty()) {
            settings.beginGroup(group);
        }

        const QVariant &v = settings.value(key);
        if (v.isValid()) {
            T t = v.value<T>();
            return t;
        }
    }

    return failback.value<T>();
}

inline QPixmap loadPixmap(const QString &path)
{
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(path, devicePixelRatio, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(path);
    }

    return pixmap;
}

#endif // UTILS_H
