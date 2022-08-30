// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "loginedin.h"
#include "downloadurl.h"
#include "avatarwidget.h"
#include "modules/sync/syncmodel.h"

#include <QDir>
#include <QLabel>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

LoginedIn::LoginedIn(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
    , m_downloader(nullptr)
    , m_avatarPath(QString("%1/.cache/deepin/dde-control-center/sync").arg(getenv("HOME")))
    , m_avatar(new AvatarWidget)
    , m_username(new QLabel)
{
    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_avatar->setFixedSize(QSize(100, 100));
    m_username->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

LoginedIn::~LoginedIn()
{
    if (m_downloader != nullptr)
        m_downloader->deleteLater();
}

void LoginedIn::onUserInfoChanged(const QVariantMap &infos)
{
    const bool isLogind = !infos["Username"].toString().isEmpty();
    const QString region = infos["Region"].toString();
    QString clasName = metaObject()->className();

    if (!isLogind)
        return;
    if (((clasName == "dccV20::sync::LogoutPage") && (region == "CN"))
            || ((clasName == "dccV20::sync::IndexPage") && (region != "CN")))
        return;

    m_username->setText(m_model->userDisplayName());
    QString avatarPath = QString("%1%2").arg(m_avatarPath).arg(m_model->userDisplayName());
    QDir dir;
    dir.mkpath(avatarPath);
    QString profile_image = infos.value("ProfileImage").toString();
    qDebug() << " ProfileImage = " << profile_image << ", avatarPath " << avatarPath;
    if (profile_image.isEmpty())
        return;

    if (m_downloader == nullptr)
        m_downloader = new DownloadUrl;

    connect(m_downloader, &DownloadUrl::fileDownloaded, this, [this](const QString &fileName) {
        qDebug() << "downloaded filename = " << fileName;
        m_avatar->setAvatarPath(fileName);
    });

    m_downloader->downloadFileFromURL(profile_image, avatarPath);
}

void LoginedIn::setModel(dcc::cloudsync::SyncModel *model)
{
    m_model = model;
}
