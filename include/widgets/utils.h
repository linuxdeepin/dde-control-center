#ifndef UTILS_H
#define UTILS_H
#include "interface/namespace.h"

#include <DSysInfo>
#include <DPlatformHandle>

#include <QMargins>
#include <QSize>
#include <QVariant>
#include <QSettings>
#include <QMetaMethod>
#include <QString>
#include <QLocale>
#include <QFile>
#include <QSettings>
#include <QImageReader>
#include <QGuiApplication>
#include <QIcon>

DCORE_USE_NAMESPACE

const QMargins ZeroMargins(0, 0, 0, 0);
const QMargins TenMargins(10, 10, 10, 10);

const int ComboxWidgetHeight = 48;
const int SwitchWidgetHeight = 36;
const int ComboxTitleWidth = 110;

const qint32 ActionIconSize=30;//大图标角标大小
const qint32 ActionListSize=26;//list图标角标大小

template <typename T>
T valueByQSettings(const QStringList& configFiles,
                   const QString&     group,
                   const QString&     key,
                   const QVariant&    failback)
{
    for (const QString& path : configFiles) {
        QSettings settings(path, QSettings::IniFormat);
        if (!group.isEmpty()) {
            settings.beginGroup(group);
        }

        const QVariant& v = settings.value(key);
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
