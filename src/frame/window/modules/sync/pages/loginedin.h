// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QWidget>

namespace dcc {
namespace cloudsync {
class SyncModel;
}
}

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace sync {
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
    void setModel(dcc::cloudsync::SyncModel *model);

protected:
    dcc::cloudsync::SyncModel *m_model;
    DownloadUrl *m_downloader;
    QString m_avatarPath;
    AvatarWidget *m_avatar;
    QLabel *m_username;
};
}
}
