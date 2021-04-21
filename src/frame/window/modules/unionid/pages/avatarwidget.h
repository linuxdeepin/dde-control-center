/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: liumaochuan <liumaochuan@uniontech.com>
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
#pragma once

#include "interface/namespace.h"

// qt
#include <QLabel>
#include <QResizeEvent>

namespace DCC_NAMESPACE {
namespace unionid {
// 头像类
class AvatarWidget : public QLabel
{
    Q_OBJECT
public:
    explicit AvatarWidget(QWidget *parent = nullptr);
    explicit AvatarWidget(const QString &avatar, QWidget *parent = nullptr);

    // 头像路径
    const QString avatarPath() const;
    // 设置头像图片路径
    void setAvatarPath(const QString &avatar, bool isUrl = true);

Q_SIGNALS:
    void toTellUserAvatar(QPixmap);

public Q_SLOTS:
    // 读取从Url中读取的数据
    void readAvatarFromUrl();

protected:
    // 将label设置为圆形
    void paintEvent(QPaintEvent *e);
private:
    QPixmap m_avatar;
    QString m_avatarPath;
};
}
}


