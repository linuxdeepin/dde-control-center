/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     zorowk <near.kingzero@gmail.com>
 *
 * Maintainer: zorowk <near.kingzero@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bubbletool.h"
#include "actionbutton.h"
#include "appicon.h"
#include "notificationentity.h"

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QSettings>
#include <QTextCodec>
#include <QDBusArgument>

#include <DDesktopEntry>

DCORE_USE_NAMESPACE

static const QStringList HintsOrder {
    "desktop-entry",
    "image-data",
    "image-path",
    "image_path",
    "icon_data"
};

inline void BubbleTool::copyLineRGB32(QRgb *dst, const char *src, int width)
{
    const char *end = src + width * 3;
    for (; src != end; ++dst, src += 3) {
        *dst = qRgb(src[0], src[1], src[2]);
    }
}

inline void BubbleTool::copyLineARGB32(QRgb *dst, const char *src, int width)
{
    const char *end = src + width * 4;
    for (; src != end; ++dst, src += 4) {
        *dst = qRgba(src[0], src[1], src[2], src[3]);
    }
}

QImage BubbleTool::decodeNotificationSpecImageHint(const QDBusArgument &arg)
{
    int width, height, rowStride, hasAlpha, bitsPerSample, channels;
    QByteArray pixels;
    char *ptr;
    char *end;

    arg.beginStructure();
    arg >> width >> height >> rowStride >> hasAlpha >> bitsPerSample >> channels >> pixels;
    arg.endStructure();
    //qDebug() << width << height << rowStride << hasAlpha << bitsPerSample << channels;

#define SANITY_CHECK(condition) \
    if (!(condition)) { \
        qWarning() << "Sanity check failed on" << #condition; \
        return QImage(); \
    }

    SANITY_CHECK(width > 0);
    SANITY_CHECK(width < 2048);
    SANITY_CHECK(height > 0);
    SANITY_CHECK(height < 2048);
    SANITY_CHECK(rowStride > 0);

#undef SANITY_CHECK

    QImage::Format format = QImage::Format_Invalid;
    void (*fcn)(QRgb *, const char *, int) = nullptr;
    if (bitsPerSample == 8) {
        if (channels == 4) {
            format = QImage::Format_ARGB32;
            fcn = copyLineARGB32;
        } else if (channels == 3) {
            format = QImage::Format_RGB32;
            fcn = copyLineRGB32;
        }
    }
    if (format == QImage::Format_Invalid) {
        qWarning() << "Unsupported image format (hasAlpha:" << hasAlpha << "bitsPerSample:" << bitsPerSample << "channels:" << channels << ")";
        return QImage();
    }

    QImage image(width, height, format);
    ptr = pixels.data();
    end = ptr + pixels.length();
    for (int y = 0; y < height; ++y, ptr += rowStride) {
        if (ptr + channels * width > end) {
            qWarning() << "Image data is incomplete. y:" << y << "height:" << height;
            break;
        }
        fcn((QRgb *)image.scanLine(y), ptr, width);
    }

    return image;
}

// Each even element in the list (starting at index 0) represents the identifier for the action.
// Each odd element in the list is the localized string that will be displayed to the user.
QString BubbleTool::processActions(ActionButton *action, QStringList action_list)
{
    action->clear();

    QString default_action;
    // the "default" is identifier for the default action
    if (action_list.contains("default")) {
        const int index = action_list.indexOf("default");
        default_action = action_list[index];
        // Default action does not need to be displayed, removed from the list
        action_list.removeAt(index + 1);
        action_list.removeAt(index);
    }

    action->addButtons(action_list);
    action->setVisible(!action_list.isEmpty());

    return default_action;
}

void BubbleTool::saveImg(const QImage &image, uint id)
{
    QDir dir;
    dir.mkdir(CachePath);

    image.save(CachePath + QString::number(id) + ".png");
}

QPixmap BubbleTool::converToPixmap(AppIcon *icon, const QDBusArgument &value, uint id)
{
    // use plasma notify source code to conver photo, solving encoded question.
    const QImage &img = BubbleTool::decodeNotificationSpecImageHint(value);
    saveImg(img, id);
    return QPixmap::fromImage(img).scaled(icon->width(), icon->height(),
                                          Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation);
}

void BubbleTool::processIconData(AppIcon *icon, EntityPtr entity)
{
    const QVariantMap &hints = entity->hints();
    QString imagePath;
    QPixmap imagePixmap;

    for (const QString &hint : HintsOrder) {
        const QVariant &source = hints.contains(hint) ? hints[hint] : QVariant();

        if (source.isNull()) continue;
        if (source.canConvert<QDBusArgument>()) {
            QDBusArgument argument = source.value<QDBusArgument>();
            imagePixmap = converToPixmap(icon, argument, entity->id());
            break;
        }

        imagePath = source.toString();
    }
    if (!imagePixmap.isNull()) {
        icon->setPixmap(imagePixmap);
    } else {
        icon->setIcon(imagePath.isEmpty() ? entity->appIcon() : imagePath, entity->appName());
    }
}

const QString BubbleTool::getDeepinAppName(const QString &name)
{
    QString settingFile = "/usr/share/applications/" + name + ".desktop";
    DDesktopEntry desktop(settingFile);

    // desktop文件语言配置规则不固定,通过判断 长语言名/短语言名 获取正确的系统语言对应的应用名
    QString localKey = "default";
    const QStringList &keys = desktop.keys();
    QString language = QLocale::system().name();
    QString shortLanguage = QLocale::system().bcp47Name();
    if (!keys.filter(language).isEmpty()) {
        localKey = language;
    } else if (!keys.filter(shortLanguage).isEmpty()) {
        localKey = shortLanguage;
    }

    if (desktop.contains("X-Deepin-Vendor")) {
        if (desktop.stringValue("X-Deepin-Vendor") == "deepin") {
            return desktop.localizedValue("GenericName", localKey, "Desktop Entry", name);
        }
    }

    return desktop.localizedValue("Name", localKey, "Desktop Entry", name);
}

void BubbleTool::actionInvoke(const QString &actionId, EntityPtr entity)
{
    qDebug() << "actionId:" << actionId;
    QMap<QString, QVariant> hints = entity->hints();
    QMap<QString, QVariant>::const_iterator i = hints.constBegin();
    while (i != hints.constEnd()) {
        QStringList args = i.value().toString().split(",");
        if (!args.isEmpty()) {
            QString cmd = args.first(); //命令
            args.removeFirst();
            if (i.key() == "x-deepin-action-" + actionId) {
                QProcess::startDetached(cmd, args); //执行相关命令
            }
        }
        ++i;
    }
}
