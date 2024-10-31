//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class ImageHelper : public QObject
{
    Q_OBJECT
public:
    explicit ImageHelper(QObject *parent = nullptr);

    Q_INVOKABLE bool isDarkType(const QImage &img);
};
