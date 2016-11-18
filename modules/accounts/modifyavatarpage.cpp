#include "modifyavatarpage.h"

#include <QDebug>

ModifyAvatarPage::ModifyAvatarPage(User *user, QWidget *parent)
    : ContentWidget(parent),

      m_avatarsLayout(new QGridLayout)
{
    m_avatarsLayout->setSpacing(0);
    m_avatarsLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *avatarsWidget = new QWidget;
    avatarsWidget->setLayout(m_avatarsLayout);

    setContent(avatarsWidget);
    setTitle(user->name());

    connect(user, &User::avatarListChanged, this, &ModifyAvatarPage::updateAvatarList);

    updateAvatarList(user->avatarList());
}

void ModifyAvatarPage::updateAvatarList(const QList<QString> &avatars)
{
    // clear old data
    while (QLayoutItem *item = m_avatarsLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    // append new data
    for (int i(0); i != avatars.size(); ++i)
    {
        // TODO: optimize
        QLabel *l = new QLabel;
        l->setPixmap(QPixmap(avatars[i].right(avatars[i].length() - 7)));

        m_avatarsLayout->addWidget(l, i / 4, i % 4);
    }
}
