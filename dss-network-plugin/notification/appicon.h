/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
