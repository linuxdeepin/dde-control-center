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
    int r = 0, g = 0, b = 0;
    for (int i = 0; i < img.width(); i++)
        for (int j = 0; j < img.height(); j++) {
            r += qRed(img.pixel(i, j));
            g += qGreen(img.pixel(i, j));
            b += qBlue(img.pixel(i, j));
        }
    auto size = img.width() * img.height();
    float luminance = 0.299 * r / size + 0.587 * g / size + 0.114 * b / size;
    return qRound(luminance) <= 170;
}
