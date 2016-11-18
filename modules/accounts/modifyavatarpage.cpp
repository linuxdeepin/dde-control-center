#include "modifyavatarpage.h"
#include "avatarwidget.h"
#include "translucentframe.h"

#include <QDebug>

using namespace dcc;

ModifyAvatarPage::ModifyAvatarPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_userInter(user),

      m_avatarsLayout(new QGridLayout)
{
    m_avatarsLayout->setSpacing(0);
    m_avatarsLayout->setContentsMargins(0, 0, 0, 0);

    TranslucentFrame *avatarsWidget = new TranslucentFrame;
    avatarsWidget->setLayout(m_avatarsLayout);
    avatarsWidget->setStyleSheet("background-color: transparent;");

    setContent(avatarsWidget);
    setTitle(user->name());

    connect(user, &User::avatarListChanged, this, &ModifyAvatarPage::updateAvatarList);

    updateAvatarList(user->avatars());
}

void ModifyAvatarPage::updateAvatarList(const QList<QString> &avatars)
{
    // clear old data
    while (QLayoutItem *item = m_avatarsLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    const QString current = m_userInter->currentAvatar();

    // append avatars
    int count = 0;
    for (auto avatar : avatars)
    {
        AvatarWidget *w = new AvatarWidget(avatar, this);
        w->setSelected(avatar == current);

        m_avatarsLayout->addWidget(w, count / 4, count % 4, Qt::AlignCenter);
        ++count;
    }
}
