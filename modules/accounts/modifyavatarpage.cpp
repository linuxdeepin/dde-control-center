/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "modifyavatarpage.h"
#include "avatarwidget.h"
#include "translucentframe.h"

#include <QDebug>
#include <QFileDialog>

#include <dimagebutton.h>

using namespace dcc::widgets;
using namespace dcc::accounts;

DWIDGET_USE_NAMESPACE

ModifyAvatarPage::ModifyAvatarPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_userInter(user),
      m_avatarsLayout(new QGridLayout)
{
    m_avatarsLayout->setSpacing(0);
    m_avatarsLayout->setVerticalSpacing(15);
    m_avatarsLayout->setContentsMargins(0, 10, 0, 0);

    TranslucentFrame *avatarsWidget = new TranslucentFrame;
    avatarsWidget->setLayout(m_avatarsLayout);

    setContent(avatarsWidget);
    setTitle(user->name());

    connect(user, &User::avatarListChanged, this, &ModifyAvatarPage::updateAvatarList);
    connect(user, &User::currentAvatarChanged, this, &ModifyAvatarPage::updateAvatarList);

    updateAvatarList();
}

void ModifyAvatarPage::appendAvatar(const QString &avatar, const int index, const bool selected, const bool deletable)
{
    AvatarWidget *w = new AvatarWidget(avatar, this);
    w->setSelected(selected);
    w->setDeletable(deletable);

    connect(w, &AvatarWidget::clicked, [=] (const QString &iconPath) { emit requestSetAvatar(m_userInter, iconPath); });
    if (deletable)
        connect(w, &AvatarWidget::requestDelete, [=](const QString &iconPath) { emit requestDeleteAvatar(m_userInter, iconPath); });

    m_avatarsLayout->addWidget(w, index / 4, index % 4, Qt::AlignCenter);
}

void ModifyAvatarPage::updateAvatarList()
{
    const auto avatars = m_userInter->avatars();
    Q_ASSERT(avatars.size() >= 14);

    // clear old data
    while (QLayoutItem *item = m_avatarsLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    const QString currentAvatar = m_userInter->currentAvatar();

    // append system avatars
    int count = 0;
    for (int i(0); i != 14; ++i)
    {
        appendAvatar(avatars[i], count, currentAvatar == avatars[i]);
        ++count;
    }

    // append custom avatar
    if (avatars.size() > 14)
    {
        appendAvatar(avatars.last(), count, currentAvatar == avatars.last(), currentAvatar != avatars.last());
        ++count;
    }

    DImageButton *btn = new DImageButton;
    btn->setAccessibleName("add_avatar");
    btn->setNormalPic(":/accounts/themes/common/icons/add_avatar_normal.png");
    btn->setHoverPic(":/accounts/themes/common/icons/add_avatar_hover.png");
    btn->setPressPic(":/accounts/themes/common/icons/add_avatar_press.png");
    connect(btn, &DImageButton::clicked, [=] { emit requestAddNewAvatar(m_userInter); });
    m_avatarsLayout->addWidget(btn, count / 4, count % 4, Qt::AlignCenter);
}
