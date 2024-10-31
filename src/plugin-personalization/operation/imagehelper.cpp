//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include <QImage>
#include <QQmlInfo>

#include "imagehelper.h"

ImageHelper::ImageHelper(QObject *parent)
    : QObject{ parent }
{
}

bool ImageHelper::isDarkType(const QImage &img)
{
    if (img.width() == 2 && img.height() == 2) {
        int r = 0, g = 0, b = 0;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++) {
                r += qRed(img.pixel(i, j));
                g += qGreen(img.pixel(i, j));
                b += qBlue(img.pixel(i, j));
            }
        float luminance = 0.299 * r / 4 + 0.587 * g / 4 + 0.114 * b / 4;
        return qRound(luminance) <= 170;
    }

    qmlWarning(this) << "ImageHelper only support 2*2 image!";
    return true;
}
