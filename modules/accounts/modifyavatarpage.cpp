#include "modifyavatarpage.h"
#include "avatarwidget.h"
#include "translucentframe.h"

#include <QDebug>
#include <QFileDialog>

#include <dimagebutton.h>

using namespace dcc;

DWIDGET_USE_NAMESPACE

ModifyAvatarPage::ModifyAvatarPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_userInter(user),
      m_avatarsLayout(new QGridLayout)
{
    m_avatarsLayout->setSpacing(0);
    m_avatarsLayout->setVerticalSpacing(15);
    m_avatarsLayout->setContentsMargins(0, 0, 0, 0);

    TranslucentFrame *avatarsWidget = new TranslucentFrame;
    avatarsWidget->setLayout(m_avatarsLayout);

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

        connect(w, &AvatarWidget::clicked, [=] (const QString &iconPath) { emit requestSetAvatar(m_userInter, iconPath); });

        m_avatarsLayout->addWidget(w, count / 4, count % 4, Qt::AlignCenter);
        ++count;
    }

    DImageButton *btn = new DImageButton;
    connect(btn, &DImageButton::clicked, [=] { emit requestAddNewAvatar(m_userInter); });
    m_avatarsLayout->addWidget(btn, count / 4, count % 4, Qt::AlignCenter);
}
