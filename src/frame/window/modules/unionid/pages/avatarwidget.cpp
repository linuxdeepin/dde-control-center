/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
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

#include "avatarwidget.h"
#include "modules/unionid/httpclient.h"
#include "window/modules/unionid/notificationmanager.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;

AvatarWidget::AvatarWidget(QWidget *parent)
    : QLabel(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
    setObjectName("AvatarWidget");
    connect(this,&AvatarWidget::toTellUserAvatar,Notificationmanager::instance(),&Notificationmanager::onUserAvatar);
}

AvatarWidget::AvatarWidget(const QString &avatar, QWidget *parent)
    : AvatarWidget(parent)
{
    setAvatarPath(avatar);
}

/*******************************************************************************
 1. @函数:    avatarPath
 2. @作者:    ut000610 戴正文
 3. @日期:    2021-03-05
 4. @说明:    获取当前头像地址
*******************************************************************************/
const QString AvatarWidget::avatarPath() const
{
    return m_avatarPath;
}

/*******************************************************************************
 1. @函数:    setAvatarPath
 2. @作者:    ut000610 戴正文
 3. @日期:    2021-03-05
 4. @说明:    根据当前设备分辨率设置图像
*******************************************************************************/
void AvatarWidget::setAvatarPath(const QString &avatar, bool isUrl)
{
    m_avatarPath = avatar;
    setAccessibleName(m_avatarPath);
    if (isUrl) {
        // url
        QNetworkReply *reply = HttpClient::instance()->getPictureFromUrl(avatar);
        connect(reply, &QNetworkReply::finished, this, &AvatarWidget::readAvatarFromUrl);
    } else {
        // 适应当前设备分辨率
        const auto ratio = devicePixelRatioF();

        // 图片地址
        m_avatar = QPixmap(avatar).scaled(size() * ratio, Qt::KeepAspectRatio, Qt::FastTransformation);
        m_avatar.setDevicePixelRatio(ratio);

        setAccessibleName(m_avatarPath);

        update();
    }
}

void AvatarWidget::setAvater(QPixmap avatar)
{
    // 适应当前设备分辨率
    const auto ratio = devicePixelRatioF();

    // 图片地址
    m_avatar = avatar.scaled(size() * ratio, Qt::KeepAspectRatio, Qt::FastTransformation);
    m_avatar.setDevicePixelRatio(ratio);

    update();
}

/*******************************************************************************
 1. @函数:    readAvatarFromUrl
 2. @作者:    ut000610 戴正文
 3. @日期:    2021-03-28
 4. @说明:    读取从Url里读取的数据
*******************************************************************************/
void AvatarWidget::readAvatarFromUrl()
{
    // 适应当前设备分辨率
    const auto ratio = devicePixelRatioF();

    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());

    QByteArray result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (!result.isEmpty()) {
//            QByteArray data_bytes = reply->readAll();
            m_avatar.loadFromData(result);
            m_avatar.scaled(size() * ratio, Qt::KeepAspectRatio, Qt::FastTransformation);
            m_avatar.setDevicePixelRatio(ratio);
            Q_EMIT toTellUserAvatar(m_avatar);
            update();

            if (!Notificationmanager::instance()->isLogin()) {
                QVariantMap map;
                map.insert("Username","");
                Q_EMIT Notificationmanager::instance()->toTellSwitchWidget(map);
            }
    }

    reply->deleteLater();
}

void AvatarWidget::paintEvent(QPaintEvent *e)
{
    //NOTE [HOME]
    Q_UNUSED(e)
    QRect r = rect();
    QPainterPath painterPath;
    painterPath.addEllipse(r);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(r, m_avatar);
}
