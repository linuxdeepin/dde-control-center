/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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
#include "widgets/translucentframe.h"

#include <QDebug>
#include <QFileDialog>

#include <dimagebutton.h>

using namespace dcc::widgets;
using namespace dcc::accounts;

DWIDGET_USE_NAMESPACE

ModifyAvatarPage::ModifyAvatarPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_userModel(user),
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
    connect(user, &User::fullnameChanged, this, &ModifyAvatarPage::updateTitle);

    updateAvatarList();

    updateTitle();
}

void ModifyAvatarPage::appendAvatar(const QString &avatar, const int index, const bool selected, const bool deletable)
{
    AvatarWidget *w = new AvatarWidget(avatar, this);
    w->setSelected(selected);
    w->setDeletable(deletable);

    connect(w, &AvatarWidget::clicked, [=] (const QString &iconPath) { Q_EMIT requestSetAvatar(m_userModel, iconPath); });
    if (deletable)
        connect(w, &AvatarWidget::requestDelete, [=](const QString &iconPath) { Q_EMIT requestDeleteAvatar(m_userModel, iconPath); });

    m_avatarsLayout->addWidget(w, index / 4, index % 4, Qt::AlignCenter);
}

void ModifyAvatarPage::updateAvatarList()
{
    const auto &avatars = m_userModel->avatars();

    // clear old data
    while (QLayoutItem *item = m_avatarsLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    const QString &currentAvatar = m_userModel->currentAvatar();

    // append system avatars
    int count = 0;
    for (int i(0); i < avatars.size() - 1; ++i)
    {
        appendAvatar(avatars[i], count, currentAvatar == avatars[i]);
        ++count;
    }

    // append last avatar, the custom avatar is start with special specified path
    if (!avatars.isEmpty())
    {
        const auto &avatar = avatars.last();

        if (avatar.startsWith("file:///var/lib/AccountsService/icons/local"))
            appendAvatar(avatars.last(), count, currentAvatar == avatars.last(), currentAvatar != avatars.last());
        else
            appendAvatar(avatar, count, currentAvatar == avatar);

        ++count;
    }

    DImageButton *btn = new DImageButton;
    btn->setAccessibleName("add_avatar");
    btn->setNormalPic(":/accounts/themes/common/icons/add_avatar_normal.svg");
    btn->setHoverPic(":/accounts/themes/common/icons/add_avatar_hover.svg");
    btn->setPressPic(":/accounts/themes/common/icons/add_avatar_press.svg");

    connect(btn, &DImageButton::clicked, [=] { Q_EMIT requestAddNewAvatar(m_userModel); });

    m_avatarsLayout->addWidget(btn, count / 4, count % 4, Qt::AlignCenter);
}

void ModifyAvatarPage::updateTitle()
{
    const QString &fullname = m_userModel->fullname();
    setTitle(fullname.isEmpty() ? m_userModel->name() : fullname);
}
