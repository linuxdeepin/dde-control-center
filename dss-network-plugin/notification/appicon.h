// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPICON_H
#define APPICON_H

#include <QLabel>
#include <QPixmap>
/*!
 * \~chinese \class AppIcon
 * \~chinese \brief 自定义控件类,在通知中心气泡中显示App图标的类
 */
class AppIcon : public QLabel
{
    Q_OBJECT

public:
    explicit AppIcon(QWidget *parent = 0);

    void setIcon(const QString &iconPath, const QString &fallback = "");
};

#endif // APPICON_H
