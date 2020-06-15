/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#include <QWidget>

namespace dcc {
namespace unionid {
class UnionidModel;
}
}

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace unionid {
class DownloadUrl;
class AvatarWidget;
class LoginedIn : public QWidget
{
    Q_OBJECT
public:
    explicit LoginedIn(QWidget *parent = nullptr);
    ~LoginedIn();

protected:
    void onUserInfoChanged(const QVariantMap &infos);
    void setModel(dcc::unionid::UnionidModel *model);

protected:
    dcc::unionid::UnionidModel *m_model;
    DownloadUrl *m_downloader;
    QString m_avatarPath;
    AvatarWidget *m_avatar;
    QLabel *m_username;
    QLabel *m_email;
};
}
}
