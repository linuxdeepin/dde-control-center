// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wallpaperthumbnailbackend_p.h"

#include <QDebug>
#include <QObject>

#include <libffmpegthumbnailer/videothumbnailer.h>

#include <exception>

class WallpaperThumbnailBackendPlugin : public QObject, public WallpaperThumbnailBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID WallpaperThumbnailBackendInterface_iid)
    Q_INTERFACES(WallpaperThumbnailBackend)

public:
    bool generate(const QString &videoPath, const QString &outputPath) const override
    {
        try {
            ffmpegthumbnailer::VideoThumbnailer thumbnailer(480, false, true, 8, false);
            thumbnailer.setThumbnailSize(480, -1);
            thumbnailer.setSeekTime("00:00:01");
            thumbnailer.generateThumbnail(videoPath.toStdString(), Png, outputPath.toStdString());
            return true;
        } catch (const std::exception &error) {
            qWarning() << "Failed to generate video thumbnail:" << videoPath << error.what();
        } catch (...) {
            qWarning() << "Failed to generate video thumbnail:" << videoPath << "unknown error";
        }

        return false;
    }
};

#include "wallpaperthumbnailbackend.moc"
